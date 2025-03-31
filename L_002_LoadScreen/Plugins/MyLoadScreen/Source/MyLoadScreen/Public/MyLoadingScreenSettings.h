// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "MyLoadingScreenSettings.generated.h"

/**
 * 
 */
UCLASS(config=Game, defaultconfig, meta=(DisplayName="Common Loading Screen"))
class MYLOADSCREEN_API UMyLoadingScreenSettings : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()
public:
	UMyLoadingScreenSettings();
	
	// The widget to load for the loading screen.
	UPROPERTY(config, EditAnywhere, Category=Display, meta=(MetaClass="/Script/UMG.UserWidget"))
	FSoftClassPath LoadingScreenWidget;

	// The z-order of the loading screen widget in the viewport stack
	UPROPERTY(config, EditAnywhere, Category=Display)
	int32 LoadingScreenZOrder = 10000;
};
