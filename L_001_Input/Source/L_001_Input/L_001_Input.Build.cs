// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class L_001_Input : ModuleRules
{
	public L_001_Input(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;
	
		PublicIncludePaths.AddRange(
			new string[] {
				"L_001_Input"
			}
		);
		
		PublicDependencyModuleNames.AddRange(new string[] { "Core",
			"CoreOnline",
			"CoreUObject",
			"ApplicationCore",
			"Engine",
			"PhysicsCore",
			"GameplayTags",
			"GameplayTasks",
			"GameplayAbilities",
			"AIModule",
			"ModularGameplay",
			"ModularGameplayActors",
			"DataRegistry",
			"ReplicationGraph",
			"GameFeatures",
			"SignificanceManager",
			"Hotfix",
			"CommonLoadingScreen",
			"Niagara",
			"AsyncMixin",
			"ControlFlows",
			"PropertyPath" });

		PrivateDependencyModuleNames.AddRange(new string[] { "InputCore",
                                                             				"Slate",
                                                             				"SlateCore",
                                                             				"RenderCore",
                                                             				"DeveloperSettings",
                                                             				"EnhancedInput",
                                                             				"NetCore",
                                                             				"RHI",
                                                             				"Projects",
                                                             				"Gauntlet",
                                                             				"UMG",
                                                             				"CommonUI",
                                                             				"CommonInput",
                                                             				"GameSettings",
                                                             				"CommonGame",
                                                             				"CommonUser",
                                                             				"GameSubtitles",
                                                             				"GameplayMessageRuntime",
                                                             				"AudioMixer",
                                                             				"NetworkReplayStreaming",
                                                             				"UIExtension",
                                                             				"ClientPilot",
                                                             				"AudioModulation",
                                                             				"EngineSettings",
                                                             				"DTLSHandlerComponent",
                                                             				"Json", });

		// Uncomment if you are using Slate UI
		// PrivateDependencyModuleNames.AddRange(new string[] { "Slate", "SlateCore" });
		
		// Uncomment if you are using online features
		// PrivateDependencyModuleNames.Add("OnlineSubsystem");

		// To include OnlineSubsystemSteam, add it to the plugins section in your uproject file with the Enabled attribute set to true
	}
}
