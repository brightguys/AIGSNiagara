#include "NiagaraGSBlueprintLibrary.h"
#include "NiagaraGSDataInterface.h"
#include "NiagaraComponent.h"
#include "NiagaraSystemInstanceController.h"

bool UNiagaraGSBlueprintLibrary::FlushGaussianSplatBuffers(UNiagaraComponent* Component)
{
    if (!Component)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("NiagaraGS: FlushGaussianSplatBuffers — null component"));
        return false;
    }

    FNiagaraSystemInstanceControllerPtr Controller = Component->GetSystemInstanceController();
    if (!Controller.IsValid() || !Controller->IsValid())
    {
        UE_LOG(LogTemp, Warning,
            TEXT("NiagaraGS: FlushGaussianSplatBuffers — component '%s' has no active system instance"),
            *Component->GetName());
        return false;
    }

    const FNiagaraSystemInstanceID InstanceID = Controller->GetSystemInstanceID();
    return UNiagaraGSDataInterface::FlushBuffersForSystemInstance(InstanceID);
}
