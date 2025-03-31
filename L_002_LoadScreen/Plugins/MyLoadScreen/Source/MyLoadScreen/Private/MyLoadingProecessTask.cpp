// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLoadingProecessTask.h"

#include "MyLoadingScreenManager.h"

UMyLoadingProecessTask* UMyLoadingProecessTask::CreateLoadingScreenProcessTask(UObject* WorldContextObject,
                                                                               const FString& ShowLoadingScreenReason)
{
	UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull);
	UGameInstance* GameInstance = World ? World->GetGameInstance() : nullptr;
	UMyLoadingScreenManager* LoadingScreenManager = GameInstance ? GameInstance->GetSubsystem<UMyLoadingScreenManager>() : nullptr;

	if (LoadingScreenManager)
	{
		// 新对象会被注册为 LoadingScreenManager 的子对象（Outer），形成 对象树。
		UMyLoadingProecessTask* NewLoadingTask = NewObject<UMyLoadingProecessTask>(LoadingScreenManager);
		NewLoadingTask->SetShowLoadingScreenReason(ShowLoadingScreenReason);

		LoadingScreenManager->RegisterLoadingProcessor(NewLoadingTask);
		
		return NewLoadingTask;
	}
	return nullptr;
}

void UMyLoadingProecessTask::Unregister()
{
	UMyLoadingScreenManager* LoadingScreenManager = Cast<UMyLoadingScreenManager>(GetOuter());
	LoadingScreenManager->UnregisterLoadingProcessor(this);
}

void UMyLoadingProecessTask::SetShowLoadingScreenReason(const FString& InReason)
{
	Reason = InReason;
}

bool UMyLoadingProecessTask::ShouldShowLoadingScreen(FString& OutReason) const
{
	OutReason = Reason;
	return true;
}
