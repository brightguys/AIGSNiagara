#include "NiagaraGSEditorModule.h"
#include "AssetToolsModule.h"
#include "GaussianSplatAssetTypeActions.h"

#define LOCTEXT_NAMESPACE "FNiagaraGSEditorModule"

void FNiagaraGSEditorModule::StartupModule()
{
    // Register asset type actions so the Content Browser knows how to display
    // and open UGaussianSplatAsset. (The .ply import factory is auto-discovered
    // by the editor through its class default object — no registration needed.)
    IAssetTools& AssetTools =
        FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools")
        .Get();

    AssetTypeActions = MakeShareable(new FGaussianSplatAssetTypeActions());
    AssetTools.RegisterAssetTypeActions(AssetTypeActions.ToSharedRef());
}

void FNiagaraGSEditorModule::ShutdownModule()
{
    // Unregister on shutdown to avoid dangling pointers
    if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
    {
        IAssetTools& AssetTools =
            FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools")
            .Get();
        AssetTools.UnregisterAssetTypeActions(AssetTypeActions.ToSharedRef());
    }
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNiagaraGSEditorModule, NiagaraGSEditor)
