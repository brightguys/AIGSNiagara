# CLAUDE.md

Guidance for AI assistants working in this repository.

## What this is

**NiagaraGS** is an Unreal Engine 5 (UE 5.6) plugin that renders **animatable
Gaussian Splats** through Niagara GPU simulation. It imports `.ply` Gaussian
Splat files, exposes the splat data to Niagara particles via a custom
**Niagara Data Interface (NDI)**, and lets the GPU simulation spawn one particle
per splat (position, scale, orientation, color/opacity, and spherical-harmonic
coefficients). Because the splats become Niagara particles, they can be animated
with the full Niagara toolset.

This repo is **only the plugin** — there is no `.uproject` here. It is meant to
be dropped into a host UE project's `Plugins/` folder (or the engine's). The
module loads at `PostEngineInit` and its name is `NiagaraGS`.

## Build & environment

- **No CI, no test suite, no build scripts in this repo.** Building happens
  inside a host UE 5.6 project — there is nothing to `make`/`npm`/`cmake` here.
  Do not invent build/test commands.
- The module is compiled by Unreal Build Tool from `Source/NiagaraGS.Build.cs`.
- `Intermediate/` and `Binaries/` are git-ignored; never commit them.
- Editor-only dependencies (`UnrealEd`, `AssetTools`, `Slate`, etc.) are added
  only when `Target.bBuildEditor` is true — keep editor code behind
  `#if WITH_EDITOR` / `#if WITH_EDITORONLY_DATA` so packaged (non-editor) builds
  still compile.
- The local `.claude/settings.local.json` allowlists reading the engine's
  bundled Niagara source (`UE_5.6/Engine/Plugins/FX/Niagara/Source/**`). That
  engine source is the reference for NDI/Niagara APIs when signatures are unclear.

## Source layout

All code lives under `Source/`, split into UE's standard `Public/` (headers) and
`Private/` (implementation), plus `Shaders/`.

| Area | Files | Role |
|------|-------|------|
| **Module** | `NiagaraGSModule.{h,cpp}` | `IModuleInterface`; registers asset type actions (editor) and the shader dir. |
| **Splat data model** | `GaussianSplatData.h` | `FGaussianSplatData` USTRUCT (one splat) + GPU packing constants. |
| **PLY parsing** | `GaussianSplatPLYParser.{h,cpp}` | Stateless `.ply` → `TArray<FGaussianSplatData>` parser (ASCII + binary LE), SH-degree detection, coordinate conversion. |
| **Imported asset** | `GaussianSplatAsset.{h,cpp}` | `UGaussianSplatAsset` UObject holding parsed splats; reimport support. |
| **Editor import** | `GaussianSplatAssetFactory.{h,cpp}` | `UFactory` that runs when a `.ply` is dropped into the Content Browser. |
| **Editor integration** | `GaussianSplatAssetTypeActions.{h,cpp}`, `GaussianSplatAssetEditor.{h,cpp}` | Content Browser type registration + a minimal summary asset editor (avoids rendering 100k+ structs). |
| **Niagara DI (game thread)** | `NiagaraGSDataInterface.{h,cpp}` | The custom `UNiagaraDataInterface`. CPU VM bindings, dynamic GPU HLSL generation, data-source resolution, flush logic. |
| **Niagara DI (render thread)** | `NiagaraGSDataInterfaceGPU.{h,cpp}` | `FNDIGaussianSplatProxy`: owns the GPU buffers/SRVs, upload + release. Shader parameter struct. |
| **Blueprint API** | `NiagaraGSBlueprintLibrary.{h,cpp}` | `FlushGaussianSplatBuffers` BP node to free VRAM on demand. |
| **Shader** | `Shaders/NiagaraGSDataInterface.ush` | **Deprecated/empty** — see note below. |

## Architecture & critical conventions

These were learned the hard way (the git history is a series of fixes for GPU
stalls and stale bytecode). Respect them.

### Data flow
1. A `.ply` is either **imported** as a `UGaussianSplatAsset` (drag into Content
   Browser → `UGaussianSplatAssetFactory`) **or** referenced as a raw on-disk
   path on the DI (`SourceFilePath`, no import). `SplatAsset` wins when both set.
2. The parser converts each splat into UE space **at import/parse time** (not at
   runtime) and stores `FGaussianSplatData`.
3. The DI resolves CPU splat data and, on the render thread, lazily uploads it
   into GPU `Buffer<float4>` SRVs owned by `FNDIGaussianSplatProxy`.
4. Niagara GPU sim reads the buffers via dynamically-generated HLSL and spawns
   one particle per splat. SH evaluation against camera direction happens in the
   material.

### The Niagara DI rules (do not break these)
- **`PerInstanceDataSize()` is intentionally 0.** Niagara never runs the
  per-instance Init/Destroy lifecycle. An earlier per-instance design churned
  (constant init/destroy) and broke the GPU path. There is **one** render-thread
  proxy per DI owning **one** buffer set.
- **`AppendCompileHash` must bump when DI codegen/layout changes.** The hidden
  per-instance "user pointer" input the translator adds depends on
  `PerInstanceDataSize()`, which is *not* otherwise part of the hash. Changing it
  without bumping the hash leaves **stale VM bytecode** that reads one register
  off (e.g. splat count landing in `Emitter.ExecutionState`). If you change CPU
  function signatures, GPU HLSL, or per-instance size, **bump the compile hash**.
- **GPU buffers self-heal in `SetShaderParameters`** (render thread): they are
  uploaded once, lazily, on the exact proxy that the GPU reads from. A zeroed
  **fallback buffer** is always bound when real buffers aren't ready or were
  flushed, so the shader never sees a null SRV.
- **Flushing VRAM is global, generation-based.** `RequestGlobalFlush()` /
  `FlushGPUBuffersVM` / the BP node bump a global atomic generation; each proxy
  releases its buffers once when the generation advances. Per-instance flush
  targeting was removed with the per-instance lifecycle — use a separate
  DI/component per system to isolate.
- **RHI release is marshalled to the render thread.** GC may run the proxy
  destructor on the game thread, so RHI releases go through
  `ENQUEUE_RENDER_COMMAND`. Keep this pattern for any new GPU resource.
- **GPU HLSL is generated dynamically in C++** (`GetParameterDefinitionHLSL` /
  `GetFunctionHLSL` in `NiagaraGSDataInterface.cpp`), **not** from a `.ush`.
  `Shaders/NiagaraGSDataInterface.ush` is deprecated and empty — a static
  `#include` of it caused Niagara compiler hangs/freezes. Do not reintroduce a
  static shader include; emit HLSL strings from C++ instead.

### Parsing performance
- Big `.ply` files have millions of splats × ~58 properties. The parser resolves
  every needed byte offset **once** in `ParseHeader` (`OffX`, `OffRest`, etc.)
  and reads by offset in the hot loop — never call `OffsetOf()` (linear search)
  per splat.
- Disk-loaded payloads are cached **process-wide** keyed by absolute path
  (`DiskCache`, guarded by `DiskCacheCS`) so a file is parsed exactly once even
  though Niagara creates/duplicates many transient DI objects.

### Coordinate / value conversion (in `GaussianSplatPLYParser.cpp`)
Applied once at parse time:
- **Position:** `xyz * 100.0` (meters → UE centimeters).
- **Scale:** `100.0 / exp(-s)` (exp-activate the log-scale, then m→cm).
- **Orientation:** PLY `rot_0..3` (wxyz) → `FQuat4f` in UE space.
- **Opacity:** sigmoid of the raw logit.
- **Color (DC):** `0.5 + SH_C0 * f_dc` where `SH_C0 = 0.28209479177387814`
  (= `1/(2√π)`).
- **Higher-order SH** (`f_rest_*`) are stored **raw**; evaluated in the material.

### SH packing (must match the material's HLSL custom node)
Constants in `GaussianSplatData.h`:
- `SH_COEFFS_PER_SPLAT = 45` (max, degree 3),
- `SH_COEFFS_PADDED = 48` (padded to a float4 boundary, 12×float4),
- `SH_VEC4_PER_SPLAT = 12`.
The GPU buffer pads **every** splat to the padded count regardless of actual SH
degree so the material indexes without per-splat branching. If you change
packing here, change the consuming material custom node too.

## Working in this codebase

- **Match the surrounding style.** Existing code uses UE conventions: `F`/`U`
  prefixes, `TArray`/`FString`/`TSharedPtr`, `UE_LOG(LogTemp, …)`, box-drawing
  comment dividers (`// ──────`), and heavy explanatory comments on the
  non-obvious threading/Niagara bits. Keep that comment density on tricky code.
- **Threading matters.** Be explicit about game vs render thread. Anything
  touching RHI/SRV/`FBufferRHIRef` belongs on the render thread or must be
  enqueued there.
- **Keep editor-only code guarded** so packaged builds compile.
- When unsure about a Niagara/NDI API, consult the engine's Niagara source
  (allowlisted at `UE_5.6/.../Niagara/Source/`) rather than guessing.

## Git & PR workflow

- Develop on the branch you were assigned for the task; create it locally if
  needed. Do **not** push to `main` without explicit permission.
- Push with `git push -u origin <branch>`; retry network failures with
  exponential backoff.
- Only open a pull request when the user explicitly asks.
- Never commit `Intermediate/` or `Binaries/`.
