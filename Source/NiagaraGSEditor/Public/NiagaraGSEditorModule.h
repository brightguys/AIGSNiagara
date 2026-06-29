#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "IAssetTypeActions.h"

/**
 * Editor-only module for NiagaraGS.
 *
 * Holds everything that depends on the editor frameworks (UnrealEd, AssetTools,
 * Slate): the .ply import factory, the Content Browser asset type actions, and
 * the summary asset editor. Keeping these out of the runtime module is what lets
 * the runtime module compile for packaged (non-editor) game targets — a UFactory
 * subclass cannot live in a Runtime module because UnrealHeaderTool still
 * reflects the UCLASS even when it is wrapped in #if WITH_EDITOR.
 */
class FNiagaraGSEditorModule : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TSharedPtr<class FGaussianSplatAssetTypeActions> AssetTypeActions;
};
