// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "MyLoadingProecessTask.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "MyLoadingScreenManager.generated.h"

/**
 * 
 */
UCLASS()
class MYLOADSCREEN_API UMyLoadingScreenManager : public UGameInstanceSubsystem,public FTickableGameObject
{
	GENERATED_BODY()
public:
	//~USubsystem interface
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;
	virtual void Deinitialize() override;
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;
	//~End of USubsystem interface

	
	//~FTickableObjectBase interface
	virtual void Tick(float DeltaTime) override;
	virtual ETickableTickType GetTickableTickType() const override;
	virtual bool IsTickable() const override;
	virtual TStatId GetStatId() const override;
	virtual UWorld* GetTickableGameObjectWorld() const override;
	//~End of FTickableObjectBase interface
	
	void RegisterLoadingProcessor(UMyLoadingProecessTask* Interface);
	void UnregisterLoadingProcessor(UMyLoadingProecessTask* Interface);
	
private:

	void UpdateLoadingScreen();
	
	void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	
	void HandlePostLoadMap(UWorld* World);

	/** 移除UI加载界面 */
	void RemoveWidgetFromViewport();

	/** 开始阻塞用户输入 */
	void StartBlockingInput();
	
	/** 停止阻塞输入事件，保证用户鼠标和键盘不会影响到游戏加载。（现在游戏中大部分鼠标可以活动的。） */
	void StopBlockingInput();

	/** 如果我们要显示加载屏幕（如果我们确实需要这么做，或者出于其他人为原因而强制显示的话），则返回真值。 */
	bool ShouldShowLoadingScreen();

	bool CheckForAnyNeedToShowLoadingScreen();

	void ShowLoadingScreen();

	void HideLoadingScreen();

	bool IsShowingInitialLoadingScreen() ;

private:
	/** 输入相关的组件 */
	TSharedPtr<IInputProcessor> InputPreProcessor;

	/**我们正在显示的加载屏幕小部件的引用（如果有的话） */
	TSharedPtr<SWidget> LoadingScreenWidget;

	/** 外部加载处理器、组件可能是延迟加载的 actor. */
	TArray<TWeakInterfacePtr<UMyLoadingProecessTask>> ExternalLoadingProcessors;

	/** 当我们位于 PreLoadMap 和 PostLoadMap 之间时为 True。 */
	bool bCurrentlyInLoadMap = false;

	/** The reason why the loading screen is up (or not) */
	FString DebugReasonForShowingOrHidingLoadingScreen;

	/** 是否应该记载屏幕UI*/
	bool bCurrentlyShowingLoadingScreen;
	double TimeLoadingScreenShown;
};


