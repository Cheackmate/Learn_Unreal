// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;
using System.Collections.Generic;

public class L_001_ServerTarget : TargetRules
{
	public L_001_ServerTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Server;

		ExtraModuleNames.AddRange(new string[] { "L_001_Input" });

		bUseChecksInShipping = true;
	}
}
