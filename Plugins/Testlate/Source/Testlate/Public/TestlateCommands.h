// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Framework/Commands/Commands.h"
#include "TestlateStyle.h"

class FTestlateCommands : public TCommands<FTestlateCommands>
{
public:

	FTestlateCommands()
		: TCommands<FTestlateCommands>(TEXT("Testlate"), NSLOCTEXT("Contexts", "Testlate", "Testlate Plugin"), NAME_None, FTestlateStyle::GetStyleSetName())
	{
	}

	// TCommands<> interface
	virtual void RegisterCommands() override;

public:
	TSharedPtr< FUICommandInfo > OpenPluginWindow;
};