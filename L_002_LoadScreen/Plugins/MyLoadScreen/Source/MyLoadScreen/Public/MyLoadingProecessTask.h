// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "MyLoadingProecessTask.generated.h"

/**
 * 
 */
UCLASS()
class MYLOADSCREEN_API UMyLoadingProecessTask : public UObject
{
	GENERATED_BODY()

public:
	
	UMyLoadingProecessTask(){};

	UFUNCTION(BlueprintCallable, meta=(WorldContext = "WorldContextObject"))
	static UMyLoadingProecessTask* CreateLoadingScreenProcessTask(UObject* WorldContextObject, const FString& ShowLoadingScreenReason);
	
	UFUNCTION(BlueprintCallable)
	void Unregister();

	UFUNCTION(BlueprintCallable)
	void SetShowLoadingScreenReason(const FString& InReason);
	
	bool ShouldShowLoadingScreen(FString& OutReason) const;

	FString Reason;
	
};
