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

        // No editor framework dependencies — this module must compile for
        // packaged (non-editor) game targets. If editor-only tooling (e.g. an
        // asset import factory) is added later, put it in a new separate
        // Editor-type module rather than here — see CLAUDE.md.

        // The module's own Private folder is already on the include path, so no
        // extra PrivateIncludePaths entry is needed here.
    }
}
