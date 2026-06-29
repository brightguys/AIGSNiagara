using UnrealBuildTool;

public class NiagaraGSEditor : ModuleRules
{
    public NiagaraGSEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(new string[]
        {
            "Core",
            "CoreUObject",
            "Engine",
            "NiagaraGS",               // runtime module: UGaussianSplatAsset, PLY parser
        });

        // This module is only ever compiled for editor targets (Type "Editor"),
        // so the editor framework is always available here.
        PrivateDependencyModuleNames.AddRange(new string[]
        {
            "UnrealEd",                // UFactory base class, import framework
            "AssetTools",              // IAssetTypeActions, asset category registration
            "ContentBrowser",          // Content Browser integration
            "Slate",                   // Layout and UI
            "SlateCore",               // Core widget and style types
            "WorkspaceMenuStructure",  // Tab category structures
            "Projects",
        });
    }
}
