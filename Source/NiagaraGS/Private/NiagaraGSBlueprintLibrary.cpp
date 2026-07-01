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

bool UNiagaraGSBlueprintLibrary::ReactivateGaussianSplatSystem(UNiagaraComponent* Component)
{
    if (!Component)
    {
        UE_LOG(LogTemp, Warning,
            TEXT("NiagaraGS: ReactivateGaussianSplatSystem — null component"));
        return false;
    }

    // Mirrors NiagaraComponentDetails.cpp's "Reset" button handler exactly.
    // Activate(true) alone (what ResetSystem() does) is not sufficient — see the
    // header comment for why.
    Component->Activate(true);
    Component->ReregisterComponent();
    return true;
}
