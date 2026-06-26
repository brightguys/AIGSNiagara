#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "NiagaraGSBlueprintLibrary.generated.h"

class UNiagaraComponent;

/**
 * Blueprint helpers for the Gaussian Splat Data Interface.
 *
 * Lets you free a running splat system's GPU buffers on demand, with full
 * control over which instance is affected — you pass the exact UNiagaraComponent,
 * so two systems running side by side are never confused.
 */
UCLASS()
class NIAGARAGS_API UNiagaraGSBlueprintLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    /**
     * Release the Gaussian Splat GPU buffers (VRAM) for the Niagara System
     * instance owned by Component. Call this once the splat data has been copied
     * into particle attributes and is no longer needed by the simulation.
     *
     * Safe to call on a system that has no Gaussian Splat DI (returns false).
     * Only the passed component's instance is flushed; other live systems keep
     * their buffers.
     *
     * @return true if a flush was queued for at least one Gaussian Splat DI.
     */
    UFUNCTION(BlueprintCallable, Category = "Gaussian Splats",
              meta = (DisplayName = "Flush Gaussian Splat GPU Buffers"))
    static bool FlushGaussianSplatBuffers(UNiagaraComponent* Component);
};
