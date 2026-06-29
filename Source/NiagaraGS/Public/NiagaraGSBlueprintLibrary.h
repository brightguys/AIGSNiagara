#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NiagaraGSBlueprintLibrary.generated.h"

class UNiagaraComponent;

/**
 * Blueprint helpers for the Gaussian Splat Data Interface.
 *
 * Lets you free splat GPU buffers on demand. NOTE: this is a GLOBAL force-flush —
 * every live Gaussian Splat system releases its VRAM on its next render, not just
 * the passed component's. The GPU reads from a duplicate DI proxy that can't be
 * targeted by component, so true per-instance flushing is done with the DI's
 * "Auto-Flush After Renders" property instead (each proxy frees only itself).
 */
UCLASS()
class NIAGARAGS_API UNiagaraGSBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Release Gaussian Splat GPU buffers (VRAM). Call this once the splat data has
     * been copied into particle attributes and is no longer needed by the sim.
     *
     * Component is currently only used to validate there is a live system instance;
     * the flush itself is GLOBAL (all splat systems release on their next render).
     * For per-system control set the DI's "Auto-Flush After Renders" property.
     *
     * @return true if a global flush was queued.
     */
    UFUNCTION(BlueprintCallable, Category = "Gaussian Splats",
              meta = (DisplayName = "Flush Gaussian Splat GPU Buffers"))
    static bool FlushGaussianSplatBuffers(UNiagaraComponent* Component);
};
