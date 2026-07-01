#pragma once
#include "CoreMinimal.h"
#include "Templates/Atomic.h"
#include "NiagaraDataInterface.h"
#include "NiagaraCommon.h"
#include "GaussianSplatData.h"
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

// Broadcast on the game thread once a ReloadFromDisk() call completes or fails.
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(
    FOnGaussianSplatReloadComplete, bool, bSuccess, int32, NewSplatCount);

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
 *   • AutoFlushAfterRenders (per-system, recommended): the proxy releases its own
 *     VRAM N renders after the upload — once the GPU spawn has copied the splats
 *     into particle attributes. Because the proxy frees ITSELF, no global state and
 *     no (unreliable) cross-proxy targeting is needed.
 *   • FlushGPUBuffers (scratchpad bool) or the Blueprint node bump a global flush
 *     generation; every proxy releases its buffers the next time it renders. This
 *     is the coarse "free all" path.
 *   • In every flushed state the zeroed fallback buffer is bound, so the shader
 *     never sees a null SRV.
 *
 * DATA SOURCE
 * ─────────────────────────────────────────────────────────────────────────────
 *   • SourceFilePath — a raw .ply path read straight off disk, no import. Parsed
 *                      once and cached process-wide (keyed by absolute path).
 * ─────────────────────────────────────────────────────────────────────────────
 */
UCLASS(EditInlineNew, Category = "Gaussian Splats",
    meta = (DisplayName = "Gaussian Splat Data Interface"))
    class NIAGARAGS_API UNiagaraGSDataInterface : public UNiagaraDataInterface
{
    GENERATED_BODY()

public:
    // ── Data source: raw .ply path on disk ────────────────────────────────
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gaussian Splats",
        meta = (DisplayName = "Source File Path (.ply)"))
    FString SourceFilePath;

    // ── Auto-flush: release the splat VRAM once the GPU spawn has copied the data
    //    into particle attributes. 0 = keep the buffers for the system's lifetime.
    //    >0 = release this many renders after the upload. The proxy releases ITSELF
    //    (it is the exact proxy the GPU reads from), so only this system is freed —
    //    no global flush, no cross-system bleed, no unreliable proxy targeting.
    //    Use 2 for a safe margin; the spawn dispatch has provably executed by then.
    //    NOTE: only safe when the material/sim reads the COPIED particle attributes,
    //    not the DI every frame. A system reset after auto-flush needs a fresh proxy
    //    (level reload / recompile) to re-upload; the CPU data stays cached for that.
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gaussian Splats|Memory",
        meta = (ClampMin = "0", DisplayName = "Auto-Flush After Renders"))
    int32 AutoFlushAfterRenders = 0;

    // ── Hot reload (async, off the game thread) ───────────────────────────
    // Re-fetches and re-parses SourceFilePath (or NewSourceFilePath if non-empty,
    // which also becomes the new SourceFilePath) on a background thread pool, then
    // atomically swaps the CPU splat data on the game thread and bumps a global
    // generation counter that the GPU proxy checks in SetShaderParameters.
    // Currently-rendered splats keep rendering unchanged until the swap — there is
    // no "goes blank" gap. GPU buffers are recreated at the new splat count on the
    // next render after the swap completes (one bounded render-thread cost, same
    // as the very first upload); the game thread itself never blocks on the
    // file read or parse.
    // NOTE: this only swaps CPU/GPU splat data. Niagara's GPU spawn burst count is
    // set once by the emitter, so respawning against the new count needs the
    // owning component reset (e.g. call ResetSystem() from OnReloadComplete).
    UFUNCTION(BlueprintCallable, Category = "Gaussian Splats",
        meta = (DisplayName = "Reload Gaussian Splats From Disk"))
    void ReloadFromDisk(const FString& NewSourceFilePath);

    // Fired on the game thread once a ReloadFromDisk() call completes or fails.
    UPROPERTY(BlueprintAssignable, Category = "Gaussian Splats")
    FOnGaussianSplatReloadComplete OnReloadComplete;

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

    // Current global data generation (read by proxies in SetShaderParameters).
    static uint64 GetDataGeneration();

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

    // ── Resolved CPU splat data (disk-loaded, process-wide cache) ─────────
    // Parses SourceFilePath once (or hits the process-wide cache). Game thread.
    void EnsureSplatDataLoaded();

    const TArray<FGaussianSplatData>* GetSplatArray() const;
    int32 GetResolvedSHDegree() const;

    // This DI's own resolved handle into the shared disk cache.
    TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe> ResolvedDiskData;

    // Resolves the disk payload: this DI's handle if set, else the process-wide
    // cache looked up by SourceFilePath, else GLastLoadedDiskData. Works on any
    // thread once parsed.
    const FGSDiskSplatData* ResolvedDiskPayload() const;

    // Process-wide parse-once cache, keyed by absolute file path.
    static FCriticalSection DiskCacheCS;
    static TMap<FString, TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe>> DiskCache;

    // Resolves ResolvedPath via DiskCache, parsing (and caching) on a miss. Safe to
    // call from any thread — DiskCache access is locked, and the parser is
    // stateless — so both the synchronous initial load (EnsureSplatDataLoaded, game
    // thread) and the background half of ReloadFromDisk (thread pool) share it.
    // Updates GLastLoadedDiskData on every resolve, but deliberately does NOT bump
    // GDataGeneration — that only happens for an explicit completed reload (see
    // ApplyReloadResult), not routine/PostLoad resolution, otherwise every
    // unrelated DI's PostLoad cache-hit would force every other proxy in the
    // project to needlessly re-upload.
    static TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe> LoadOrParseDiskPayload(const FString& ResolvedPath);

    // Background half of ReloadFromDisk(): applies a completed async reload on the
    // game thread. Discards the result if a newer ReloadFromDisk() call has since
    // superseded it (out-of-order async completion).
    void ApplyReloadResult(uint64 RequestId, TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe> NewPayload);

    // Identifies the most recent ReloadFromDisk() call on THIS instance, so a
    // stale completion can be discarded. Plain (not atomic) — only ever touched on
    // the game thread (ReloadFromDisk and ApplyReloadResult both run there); the
    // background task only carries the value captured at dispatch time.
    uint64 ReloadRequestCounter = 0;

    // Last-resort fallback for a DI instance with an EMPTY SourceFilePath — this
    // happens because Niagara's GPU-compute-script binding for this DI can be a
    // duplicate captured at system-instance init time, before the user parameter
    // override's SourceFilePath was set; that duplicate never gets refreshed, so
    // without this it self-heals to zero splats forever. Scoped as tightly as we
    // can without live per-instance correlation: cleared by RequestGlobalFlush(),
    // so deleting/reconfiguring the source and hitting Flush drops the ghost data.
    // ("Single active splat file" assumption — see class comment.)
    static TSharedPtr<const FGSDiskSplatData, ESPMode::ThreadSafe> GLastLoadedDiskData;

    // Global flush generation. A manual flush increments it; a proxy compares it to
    // the generation it last serviced and releases once when it advances.
    static TAtomic<uint64> GlobalFlushGeneration;

    // Global data generation. Bumped only by a completed ReloadFromDisk() (see
    // ApplyReloadResult), never by routine resolution. Global — like
    // GlobalFlushGeneration — for the same reason: the GPU-bound DI object in
    // SetShaderParameters may not be the one ReloadFromDisk() was called on (see
    // GLastLoadedDiskData above), so a per-instance counter would not be visible
    // to the proxy that actually needs to re-upload. Trade-off: reloading ANY
    // system also makes every OTHER unrelated system's proxy redundantly
    // re-upload its own (unchanged) data once — acceptable given the plugin's
    // existing "single active splat file assumed" coarseness elsewhere.
    static TAtomic<uint64> GDataGeneration;

    // Rising-edge latch for the scratchpad/BP FlushGPUBuffers bool so holding it
    // high requests a single (global) flush, not one per frame. Transient, per-DI.
    bool bFlushEdgeLatch = false;
};