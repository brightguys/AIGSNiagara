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

        // Editor-only modules — stripped from packaged builds
        if (Target.bBuildEditor)
        {
            PrivateDependencyModuleNames.AddRange(new string[]
            {
                "UnrealEd",               // UFactory base class, import framework
                "AssetTools",             // IAssetTypeActions, asset category registration
                "ContentBrowser",         // Content Browser integration
                "Slate",                  // Layout and UI
                "SlateCore",              // Core widget and style types
                "WorkspaceMenuStructure", // Tab category structures
            });
        }

        // The module's own Private folder is already on the include path, so no
        // extra PrivateIncludePaths entry is needed here.
    }
}
