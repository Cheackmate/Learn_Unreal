using UnrealBuildTool;

public class CommonStartupLoadingScreen : ModuleRules
{
    public CommonStartupLoadingScreen(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[]
            {
                "Core",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                "MoviePlayer",
                "PreLoadScreen",
                "DeveloperSettings"
            }
        );
    }
}