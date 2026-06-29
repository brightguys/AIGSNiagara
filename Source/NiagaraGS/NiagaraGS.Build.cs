using UnrealBuildTool;

public class NiagaraGS : ModuleRules
{
    public NiagaraGS(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "RenderCore",
            "RHI",
            "Renderer",
            "Projects",
            "InputCore",
            "Niagara",
            "NiagaraCore",      // FNiagaraSystemInstance, FNiagaraDataInterfaceProxy
        });

        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "RenderCore",
            "RHI",
            "Niagara",
            "NiagaraCore",
            "VectorVM",
            "NiagaraShader",
        });

        // Editor-only code (import factory, asset type actions, asset editor)
        // lives in the separate NiagaraGSEditor module, so this runtime module
        // has no editor framework dependencies and compiles for packaged builds.

        // The module's own Private folder is already on the include path, so no
        // extra PrivateIncludePaths entry is needed here.
    }
}
