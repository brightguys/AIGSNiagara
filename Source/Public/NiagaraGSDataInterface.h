#pragma once
#include "CoreMinimal.h"
#include "Templates/Atomic.h"
#include "NiagaraDataInterface.h"
#include "NiagaraCommon.h"
#include "GaussianSplatData.h"
#include "GaussianSplatAsset.h"
#include "NiagaraGSDataInterfaceGPU.h"
#include "NiagaraDataInterfaceRW.h"
#include "NiagaraShaderParametersBuilder.h"
#include "NiagaraGSDataInterface.generated.h"

/**
 * Parsed splat payload for one .ply file loaded straight from disk.
 * Held by a thread-safe shared pointer and cached process-wide (keyed by absolute
 * path) so a file is parsed exactly once and reused by every DI instance — Niagara
 * may create/duplicate many transient DI objects, and re-parsing a multi-million
 * splat file each time is what made disk loading "take forever".
 */
struct FGSDiskSplatData
{
    TArray<FGaussianSplatData> Splats;
    int32   SHDegree = 0;
    FString SourcePath;   // absolute path this payload was parsed from (cache key)
};

/**
 * Custom Niagara Data Interface exposing Gaussian Splat data to GPU particles.
 *
 * ARCHITECTURE
 * ─────────────────────────────────────────────────────────────────────────────
 * The DI deliberately uses PerInstanceDataSize() == 0, so Niagara never runs the
 * per-instance Init/Destroy lifecycle. (An earlier per-instance design churned —
 * constant init/destroy — which broke the GPU path and tanked performance.) The
 * render-thread proxy is created once with the DI and owns ONE GPU buffer set.
 *
 *   • SetShaderParameters() runs on the real rendering proxy. It lazily uploads
 *     the buffers once (self-heal) from the resolved CPU data, then binds them.
 *   • FlushGPUBuffers (scratchpad bool) or the Blueprint node bump a global flush
 *     generation; the proxy releases its buffers the next time it renders and
 *     binds the zeroed fallback so the shader never sees a null SRV.
 *
 * DATA SOURCE
 * ─────────────────────────────────────────────────────────────────────────────
 *   • SplatAsset      — an imported UGaussianSplatAsset (.ply dragged into UE), or
 *   • SourceFilePath  — a raw .ply path read straight off disk, no import. Parsed
 *                       once and cached process-wide.
 * SplatAsset takes priority when both are set.
 * ─────────────────────────────────────────────────────────────────────────────
 */
UCLASS(EditInlineNew, Category = "Gaussian Splats",
    meta = (DisplayName = "Gaussian Splat Data Interface"))
    class NIAGARAGS_API UNiagaraGSDataInterface : public UNiagaraDataInterface
{
    GENERATED_BODY()

public:
    // ── Data source: imported asset ───────────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gaussian Splats")
    TObjectPtr<UGaussianSplatAsset> SplatAsset;

    // ── Data source: raw .ply path on disk (used only when SplatAsset is null) ─
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gaussian Splats",
        meta = (DisplayName = "Source File Path (.ply, no import)"))
    FString SourceFilePath;

    // ── UNiagaraDataInterface interface ───────────────────────────────────
#if WITH_EDITORONLY_DATA
    virtual void GetFunctionsInternal(TArray<FNiagaraFunctionSignature>& OutFunctions) const override;
    // Bump the DI compile hash so Niagara regenerates scripts. CRITICAL: the
    // hidden per-instance "user pointer" input the translator adds to CPU
    // functions depends on PerInstanceDataSize(), which is NOT otherwise part of
    // the hash — so without this, switching that value leaves stale bytecode that
    // reads one register off (e.g. splat count landing in Emitter.ExecutionState).
    virtual bool AppendCompileHash(FNiagaraCompileHashVisitor* InVisitor) const override;
#endif
    virtual void GetVMExternalFunction(
        const FVMExternalFunctionBindingInfo& BindingInfo,
        void* InstanceData,
        FVMExternalFunction& OutFunc) override;

    virtual bool CanExecuteOnTarget(ENiagaraSimTarget Target) const override { return true; }
    virtual bool CopyToInternal(UNiagaraDataInterface* Destination) const override;
    virtual bool Equals(const UNiagaraDataInterface* Other) const override;

    // NOTE: PerInstanceDataSize() is intentionally left at the base value of 0 —
    // see the architecture note above.

    // ── Helpers ───────────────────────────────────────────────────────────
    int32 GetSplatCount() const;

    virtual void PostInitProperties() override;
    virtual void PostLoad() override;

    // ── CPU VM bindings ───────────────────────────────────────────────────
    void GetSplatCount(FVectorVMExternalFunctionContext& Context);
    void GetSplatPosition(FVectorVMExternalFunctionContext& Context);
    void GetSplatScale(FVectorVMExternalFunctionContext& Context);
    void GetSplatOrientation(FVectorVMExternalFunctionContext& Context);
    void GetSplatColor(FVectorVMExternalFunctionContext& Context);
    void GetSplatOpacity(FVectorVMExternalFunctionContext& Context);
    void GetSplatSHColor(FVectorVMExternalFunctionContext& Context);
    void GetSplatSHCoefficients(FVectorVMExternalFunctionContext& Context);

    /**
     * FlushGPUBuffers — callable from a Niagara Scratchpad / module (CPU stage).
     * Input  Flush   (bool): when true, frees the GPU splat buffers.
     * Output Success (bool): true if a flush was requested.
     * Call after the frame that copies splat data into particle attributes.
     */
    void FlushGPUBuffersVM(FVectorVMExternalFunctionContext& Context);

    // ── GPU interface ─────────────────────────────────────────────────────
#if WITH_EDITORONLY_DATA
    virtual void GetParameterDefinitionHLSL(
        const FNiagaraDataInterfaceGPUParamInfo& ParamInfo,
        FString& OutHLSL) override;
    virtual bool GetFunctionHLSL(
        const FNiagaraDataInterfaceGPUParamInfo& ParamInfo,
        const FNiagaraDataInterfaceGeneratedFunction& FunctionInfo,
        int FunctionInstanceIndex,
        FString& OutHLSL) override;
#endif
    virtual void BuildShaderParameters(
        FNiagaraShaderParametersBuilder& ShaderParametersBuilder) const override;
    virtual void SetShaderParameters(
        const FNiagaraDataInterfaceSetShaderParametersContext& Context) const override;

#if WITH_EDITOR
    virtual void PostEditChangeProperty(
        struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

    // ── Flush entry points ────────────────────────────────────────────────
    // Bumps the global flush generation; every live Gaussian Splat proxy releases
    // its VRAM the next time it renders. (Per-instance targeting was removed with
    // the per-instance lifecycle; use a separate DI/component per system to isolate.)
    static void RequestGlobalFlush();

    // Blueprint helper entry point (UNiagaraGSBlueprintLibrary). The component is
    // accepted for a future per-system flush; today it triggers a global flush.
    static bool FlushBuffersForSystemInstance(FNiagaraSystemInstanceID InstanceID);

    // Current global flush generation (read by proxies in SetShaderParameters).
    static uint64 GetFlushGeneration();

private:
    // ── Function name constants ───────────────────────────────────────────
    static const FName Name_GetSplatCount;
    static const FName Name_GetSplatPosition;
    static const FName Name_GetSplatScale;
    static const FName Name_GetSplatOrientation;
    static const FName Name_GetSplatColor;
    static const FName Name_GetSplatOpacity;
    static const FName Name_GetSplatSHColor;
    static const FName Name_GetSplatSHCoefficients;
    static const FName Name_FlushGPUBuffers;

    // ── Resolved CPU splat data (asset OR disk-loaded cache) ──────────────
    // Parses SourceFilePath once (or hits the process-wide cache). Game thread.
    void EnsureSplatDataLoaded();

    const TArray<FGaussianSplatData>* GetSplatArray() const;
    int32 GetResolvedSHDegree() const;

    // This DI's handle into the shared disk cache (null when using SplatAsset).
    TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe> ResolvedDiskData;

    // Resolves the disk payload: this DI's handle if set, else the process-wide
    // cache looked up by SourceFilePath. Works on any thread once parsed.
    const FGSDiskSplatData* ResolvedDiskPayload() const;

    // Process-wide parse-once cache, keyed by absolute file path.
    static FCriticalSection DiskCacheCS;
    static TMap<FString, TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe>> DiskCache;

    // Most recently resolved payload. The DI bound to the GPU compute script is
    // often a different, UNCONFIGURED object (no asset, no path) than the one the
    // user set up; it falls back to this so the GPU still gets real splat data.
    static TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe> GLastLoadedDiskData;

    // Global flush generation. A manual flush increments it; a proxy compares it to
    // the generation it last serviced and releases once when it advances.
    static TAtomic<uint64> GlobalFlushGeneration;

    // Edge-latch for the scratchpad FlushGPUBuffers bool so holding it high only
    // requests one flush (not one per frame). Transient, per-DI.
    bool bFlushLatch = false;
};