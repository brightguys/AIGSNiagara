# CLAUDE.md

Guidance for AI assistants working in this repository.

## What this is

**NiagaraGS** is an Unreal Engine 5 (UE 5.6) plugin that renders **animatable
Gaussian Splats** through Niagara GPU simulation. It reads `.ply` Gaussian Splat
files straight off disk, exposes the splat data to Niagara particles via a
custom **Niagara Data Interface (NDI)**, and lets the GPU simulation spawn one
particle per splat (position, scale, orientation, color/opacity, and
spherical-harmonic coefficients). Because the splats become Niagara particles,
they can be animated with the full Niagara toolset.

This repo is **only the plugin** — there is no `.uproject` here. It is meant to
be dropped into a host UE project's `Plugins/` folder (or the engine's). It ships
**one module**, `NiagaraGS` (Runtime — all code, no editor deps), loading at
`PostEngineInit`. There is currently no `.ply` import pipeline or Content
Browser integration — the DI reads a raw on-disk path (`SourceFilePath`)
directly; there is no imported-`UAsset` data source. If editor-only tooling is
added in the future (an import factory, an asset editor, etc.), put it in a
new, separate `Editor`-type module rather than in `NiagaraGS` — see the
`UCLASS`-vs-`#if WITH_EDITOR` rule below for why.

## Build & environment

- **No CI, no test suite, no build scripts in this repo.** Building happens
  inside a host UE 5.6 project — there is nothing to `make`/`npm`/`cmake` here.
  Do not invent build/test commands. (To produce a redistributable precompiled
  build, see **Packaging a precompiled plugin** below.)
- The module is compiled by Unreal Build Tool from
  `Source/NiagaraGS/NiagaraGS.Build.cs`.
- `Intermediate/` and `Binaries/` are git-ignored; never commit them.
- **Editor-only `UCLASS`es must live in a separate `Editor`-type module, not
  behind `#if WITH_EDITOR` in the runtime module.** Runtime UObjects that merely
  *need* editor-only **members/methods** can still guard those with
  `#if WITH_EDITOR` and stay in the runtime module — that is fine (e.g.
  `UNiagaraGSDataInterface::PostEditChangeProperty`). What does **not** work is
  an editor-only **`UCLASS`** (e.g. a `UFactory` subclass) in the runtime
  module: UnrealHeaderTool reflects the `UCLASS` even when the whole thing is
  wrapped in `#if WITH_EDITOR`, so packaged (`UnrealGame`) builds fail with
  "Unable to find parent class type". This plugin used to ship a second
  `NiagaraGSEditor` module for exactly this reason (a `.ply` import factory +
  Content Browser integration); that pipeline was removed (disk-path-only now),
  so there is currently only one module — but if editor-only `UCLASS` tooling
  is added again, recreate a dedicated Editor module for it.
- The local `.claude/settings.local.json` allowlists reading the engine's
  bundled Niagara source (`UE_5.6/Engine/Plugins/FX/Niagara/Source/**`). That
  engine source is the reference for NDI/Niagara APIs when signatures are unclear.

## Source layout

The module's source lives under `Source/NiagaraGS/`, in UE's canonical layout —
`Source/<Module>/<Module>.Build.cs` plus the module's `Public/` (headers) and
`Private/` (implementation). If a second module is ever added (e.g. an editor
module), give it its own sibling folder under `Source/` with its own
`.Build.cs`; do **not** put a module's `.Build.cs` directly in `Source/` itself
— with one module's source rooted at `Source/`, UBT treats a sibling module's
subfolder as part of it and never registers the second module ("Could not find
definition for module …").

```
Source/
  NiagaraGS/            (Runtime module)
    NiagaraGS.Build.cs
    Public/  Private/  Shaders/
```

**`NiagaraGS` (Runtime)** — no editor framework dependencies:

| Area | Files | Role |
|------|-------|------|
| **Module** | `NiagaraGSModule.{h,cpp}` | `IModuleInterface`; registers the shader dir. |
| **Splat data model** | `GaussianSplatData.h` | `FGaussianSplatData` USTRUCT (one splat) + GPU packing constants. |
| **PLY parsing** | `GaussianSplatPLYParser.{h,cpp}` | Stateless `.ply` → `TArray<FGaussianSplatData>` parser (ASCII + binary LE), SH-degree detection, coordinate conversion. |
| **Niagara DI (game thread)** | `NiagaraGSDataInterface.{h,cpp}` | The custom `UNiagaraDataInterface`. CPU VM bindings, dynamic GPU HLSL generation, data-source resolution, flush logic, async hot reload (`ReloadFromDisk`). |
| **Niagara DI (render thread)** | `NiagaraGSDataInterfaceGPU.{h,cpp}` | `FNDIGaussianSplatProxy`: owns the GPU buffers/SRVs, upload + release. Shader parameter struct. |
| **Blueprint API** | `NiagaraGSBlueprintLibrary.{h,cpp}` | `FlushGaussianSplatBuffers` BP node to free VRAM on demand; `ReactivateGaussianSplatSystem` to make a completed `ReloadFromDisk()` visible (see DI rules below — plain `ResetSystem()` is not enough). |
| **Shader** | `Shaders/NiagaraGSDataInterface.ush` | **Deprecated/empty** — see note below. |

## Architecture & critical conventions

These were learned the hard way (the git history is a series of fixes for GPU
stalls and stale bytecode). Respect them.

### Data flow
1. The DI is configured with a raw on-disk path (`SourceFilePath`) — there is no
   import step and no `UAsset` data source.
2. The parser converts each splat into UE space **at parse time** (not at
   runtime) and stores `FGaussianSplatData`. Parsed payloads are cached
   process-wide, keyed by absolute path (`DiskCache`), so a file is parsed once
   no matter how many DI objects/instances reference it.
3. The DI resolves CPU splat data and, on the render thread, lazily uploads it
   into GPU `Buffer<float4>` SRVs owned by `FNDIGaussianSplatProxy`.
4. Niagara GPU sim reads the buffers via dynamically-generated HLSL and spawns
   one particle per splat. SH evaluation against camera direction happens in the
   material.
5. `ReloadFromDisk(NewPath)` hot-swaps the source without a game-thread hitch:
   the fetch+parse runs on a background thread pool (`Async`/`AsyncTask`), and
   only the completion (game thread) atomically swaps `ResolvedDiskData` and
   bumps `GDataGeneration`. The currently-rendered data is untouched until that
   swap — no "goes blank" gap. See the DI rules below for how the GPU side picks
   this up.

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
- **The DI object bound to the GPU compute script is often a *different*,
  UNCONFIGURED duplicate** (empty `SourceFilePath`) than the one the user
  actually set up — a real Niagara quirk, not a bug in this plugin (CPU-bound
  function calls always resolve against the live, correctly-configured object;
  the GPU-bound copy does not reliably receive later changes). Self-heal on that
  unconfigured object therefore falls back to `GLastLoadedDiskData` — a
  process-wide "most recently loaded" pointer — so the GPU still gets real data.
  **This is deliberately a single, coarse, "single active splat file assumed"
  fallback, not scoped per-system**: removing it entirely breaks GPU upload for
  every correctly-configured system (confirmed by regression — do not remove
  it), but it also means an unrelated/never-configured DI can transiently show
  another system's splats until something loads its own. `RequestGlobalFlush()`
  (the BP node / scratchpad `FlushGPUBuffers`) clears it, so flushing after
  deleting/reconfiguring a source drops the stale guess. Do not "fix" this by
  reverting to a bare `nullptr` fallback without first confirming — with a live
  editor test, GPU emitter rendering, not just the CPU path — that whatever
  replaces it still reaches the actual GPU-bound duplicate object.
- **Flushing VRAM is global, generation-based.** `RequestGlobalFlush()` /
  `FlushGPUBuffersVM` / the BP node bump a global atomic generation; each proxy
  releases its buffers once when the generation advances. Per-instance flush
  targeting was removed with the per-instance lifecycle — use a separate
  DI/component per system to isolate.
- **Hot reload (`ReloadFromDisk`) re-uploads via the same global-generation
  pattern as flush, for the same reason.** `ApplyReloadResult` (the async
  completion, game thread) bumps `GDataGeneration` — a global `TAtomic<uint64>`,
  not a per-instance counter — because the GPU-bound object `SetShaderParameters`
  runs on may not be the instance `ReloadFromDisk()` was called on (see the
  GLastLoadedDiskData bullet above). `SetShaderParameters` re-uploads once when it
  sees the generation advance past the proxy's own `UploadedDataGeneration`, even
  if buffers are already ready. Trade-off: reloading any one system also makes
  every other unrelated system's proxy redundantly re-upload its own unchanged
  data once — accepted for the same "single active splat file assumed" reason as
  the fallback above. `ReloadRequestCounter`, by contrast, IS per-instance (plain
  `uint64`, game-thread-only) — it only discards a stale out-of-order completion
  on the same object, so it doesn't need to be global.
  **On a generation-mismatch re-upload, read `GLastLoadedDiskData` directly —
  never `GetSplatArray()`/`this`.** Confirmed by regression: after one successful
  load, the GPU-bound duplicate already has a non-empty (but stale)
  `SourceFilePath`/`ResolvedDiskData` from before the reload — Niagara never
  refreshes a duplicate's properties once created, only `CopyToInternal`s them at
  creation time. `GetSplatArray()` on that stale duplicate does NOT fail (which
  would at least be visible); it "succeeds" by resolving the OLD file's still-live
  `DiskCache` entry (never evicted, keyed by the duplicate's own stale path) and
  silently re-uploads the same old data. Symptom looked like "reload does
  nothing": `OnReloadComplete` fired with the correct new splat count
  (CPU/VM functions always resolve against the live object, so that part is
  correct), but the GPU buffer never changed. `GLastLoadedDiskData` is reliably
  the freshest payload as of the most recent successful load, so bypass per-object
  resolution entirely for this specific branch.
  `ReloadFromDisk` only swaps CPU/GPU data; it does not reset the Niagara system,
  so the GPU spawn burst count (set once by the emitter) won't itself change —
  react to `OnReloadComplete` and call `ResetSystem()` on the owning component.
- **Making the new data visible needs `Activate(true)` AND `ReregisterComponent()`
  — `ResetSystem()` alone is NOT enough, confirmed by regression.** After the
  `GLastLoadedDiskData` fix above, the GPU buffer demonstrably gets the new splats
  (`UE_LOG` "Uploaded N splats" fires with the correct new count right after
  `ReloadFromDisk`, before touching any reset) — but calling `ResetSystem()`
  (`Activate(true)` alone) from Blueprint still visually kept showing the old
  splats. Only the Niagara Component's Details-panel "Reset" button worked, and
  its handler (`FNiagaraComponentDetails::OnResetSelectedSystem` in
  `NiagaraComponentDetails.cpp`) does `Activate(true)` **then**
  `ReregisterComponent()` — the extra step actually responsible for making
  particles respawn against the new buffer (particles copy their attributes from
  the DI buffer once, at spawn time — see `AutoFlushAfterRenders` above — so nothing
  new appears until something forces an actual respawn). `ReregisterComponent()`
  is not `BlueprintCallable` in the engine, so
  `UNiagaraGSBlueprintLibrary::ReactivateGaussianSplatSystem(Component)` wraps
  both calls for Blueprint. Do not "simplify" this back down to just
  `ResetSystem()`/`Activate(true)` without re-confirming in a live GPU-emitter
  test — the `ReregisterComponent()` call is load-bearing.
- **The `UCLASS` needs `BlueprintType`, or none of this is reachable from
  Blueprint.** Blueprint's "Cast To" node requires the target class to declare
  `BlueprintType`; without it there is no way to cast a generic
  `UNiagaraDataInterface*` down to this subclass, so `ReloadFromDisk`,
  `OnReloadComplete`, or any future `BlueprintCallable` added here are
  unreachable from a BP graph (confirmed by regression — this is why
  `FlushGaussianSplatBuffers` originally had to be a static
  `UNiagaraGSBlueprintLibrary` function taking a `UNiagaraComponent*` instead of
  a method called directly on the DI). The built-in
  `UNiagaraDataInterfaceRenderTarget2D` proves a concrete DI subclass can safely
  be `BlueprintType` despite the abstract `UNiagaraDataInterface` base not being
  one. To get the object in Blueprint: `UNiagaraComponent::GetDataInterface(Name)`
  (deprecated since 4.27, still functional in 5.6, and there is no non-deprecated
  `BlueprintCallable` generic replacement in `UNiagaraFunctionLibrary` — its
  equivalents are C++-only templates) → Cast to the DI's Blueprint type.
- **RHI release is marshalled to the render thread.** GC may run the proxy
  destructor on the game thread, so RHI releases go through
  `ENQUEUE_RENDER_COMMAND`. Keep this pattern for any new GPU resource.
- **GPU HLSL is generated dynamically in C++** (`GetParameterDefinitionHLSL` /
  `GetFunctionHLSL` in `NiagaraGSDataInterface.cpp`), **not** from a `.ush`.
  `Shaders/NiagaraGSDataInterface.ush` is deprecated and empty — a static
  `#include` of it caused Niagara compiler hangs/freezes. Do not reintroduce a
  static shader include; emit HLSL strings from C++ instead.

### Parsing & GPU-upload performance
- Big `.ply` files have millions of splats × ~58 properties. The parser resolves
  every needed byte offset **once** in `ParseHeader` (`OffX`, `OffRest`, etc.)
  and reads by offset in the hot loop — never call `OffsetOf()` (linear search)
  per splat.
- Disk-loaded payloads are cached **process-wide** keyed by absolute path
  (`DiskCache`, guarded by `DiskCacheCS`) so a file is parsed exactly once even
  though Niagara creates/duplicates many transient DI objects.
- **The CPU→GPU float4 repacking (`PackSplatsForGPU`) happens ONCE, at load
  time, in `LoadOrParseDiskPayload` — never in `FNDIGaussianSplatProxy::UploadData`
  (render thread).** This used to be a per-splat CPU loop (plus a `SetNumZeroed`
  allocation of `Count × SH_VEC4_PER_SPLAT` float4s — ~192MB for a 1M-splat file's
  SH buffer alone) run synchronously on the render thread on every single upload,
  including every `ReloadFromDisk()` swap — a real, measurable hitch **independent
  of** the disk fetch/parse (which was already correctly backgrounded), confirmed
  by regression: reload felt just as heavy as the full `ReinitializeSystem` path
  it was meant to avoid. Packed float4 arrays now live on `FGSDiskSplatData`
  (`PackedPositions/Scales/Rotations/ColorOpacity/SH`) alongside the parsed
  `Splats`, computed once per file (same "parse once, cache process-wide"
  cache-hit path already used for parsing) regardless of which thread resolves a
  cache miss. `UploadData()` now does RHI buffer creation + a straight `memcpy`
  only — the unavoidable part of getting bytes onto the GPU. If you add a new
  per-splat GPU attribute, pack it here too; do not reintroduce a per-splat loop
  inside `UploadData`.
- Remaining upload cost is proportional to splat count (buffer creation + memcpy
  for 5 buffers) and is paid once per swap on the render thread — there is no
  known way to make RHI resource creation itself free. If reload cadence is ever
  high enough for this to matter, the next lever is pre-allocating buffers for a
  max splat count and reusing them (`RHIUpdateBuffer` in place) instead of
  releasing + recreating on every swap — not implemented; would trade fixed
  extra VRAM for less RHI churn.

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

## Packaging a precompiled plugin

To produce a redistributable, drop-in build (precompiled `Binaries/` so testers
just unzip into `Project/Plugins/` with no compile step), run the engine's
`BuildPlugin` automation tool. From any shell on Windows:

```powershell
& "C:\Program Files\Epic Games\UE_5.6\Engine\Build\BatchFiles\RunUAT.bat" BuildPlugin `
    -Plugin="C:\Users\deety\Documents\Unreal Projects\GSPlugin\Plugins\NiagaraGS\NiagaraGS.uplugin" `
    -Package="C:\Users\deety\Documents\Unreal Projects\GSPlugin\Packaged\NiagaraGS" `
    -TargetPlatforms=Win64 -Rocket
```

- `-Rocket` builds against the installed (Launcher) engine so the binaries load
  for testers on the same UE 5.6 Launcher build.
- It builds **both** the editor and the packaged-game (`UnrealGame`) targets, so
  it's also the quickest way to catch the editor-vs-runtime module mistakes
  described above.
- The output folder is fully self-contained. For distribution you can delete
  `Intermediate/` and the `Binaries/**/*.pdb` symbols (regenerable / not needed),
  then zip the `NiagaraGS` folder. `BuildPlugin` stamps `"Installed": true` in the
  output `.uplugin` so the editor loads the binaries without rebuilding.

## Working in this codebase

- **Match the surrounding style.** Existing code uses UE conventions: `F`/`U`
  prefixes, `TArray`/`FString`/`TSharedPtr`, `UE_LOG(LogTemp, …)`, box-drawing
  comment dividers (`// ──────`), and heavy explanatory comments on the
  non-obvious threading/Niagara bits. Keep that comment density on tricky code.
- **Threading matters.** Be explicit about game vs render thread. Anything
  touching RHI/SRV/`FBufferRHIRef` belongs on the render thread or must be
  enqueued there.
- **Keep editor-only `UCLASS`es in a dedicated Editor module** (not in the
  runtime module) so packaged builds compile — see Build & environment above
  for the `UCLASS`-vs-`#if WITH_EDITOR` rule.
- When unsure about a Niagara/NDI API, consult the engine's Niagara source
  (allowlisted at `UE_5.6/.../Niagara/Source/`) rather than guessing.

## Git & PR workflow

- Develop on the branch you were assigned for the task; create it locally if
  needed. Do **not** push to `main` without explicit permission.
- Push with `git push -u origin <branch>`; retry network failures with
  exponential backoff.
- Only open a pull request when the user explicitly asks.
- Never commit `Intermediate/` or `Binaries/`.
