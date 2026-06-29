#include "NiagaraGSModule.h"
#include "Interfaces/IPluginManager.h"
#include "Misc/Paths.h"
#include "ShaderCore.h"

#define LOCTEXT_NAMESPACE "FNiagaraGSModule"

void FNiagaraGSModule::StartupModule()
{
    // Register shader directory (from Step 1)
    FString PluginShaderDir = FPaths::Combine(
        IPluginManager::Get().FindPlugin(TEXT("NiagaraGS"))->GetBaseDir(),
        TEXT("Shaders")
    );
    //AddShaderSourceDirectoryMapping(TEXT("/NiagaraGS"), PluginShaderDir);

    // Editor-only registration (asset type actions, import factory) now lives in
    // the separate NiagaraGSEditor module so this runtime module compiles for
    // packaged (non-editor) game targets.
}

void FNiagaraGSModule::ShutdownModule()
{
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE(FNiagaraGSModule, NiagaraGS)