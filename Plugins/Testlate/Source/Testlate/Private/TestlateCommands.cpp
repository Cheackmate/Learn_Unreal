// Copyright Epic Games, Inc. All Rights Reserved.

#include "TestlateCommands.h"

#define LOCTEXT_NAMESPACE "FTestlateModule"

void FTestlateCommands::RegisterCommands()
{
	UI_COMMAND(OpenPluginWindow, "Testlate", "Bring up Testlate window", EUserInterfaceActionType::Button, FInputChord());
}

#undef LOCTEXT_NAMESPACE
