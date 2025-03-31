// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Subsystems/GameInstanceSubsystem.h"
#include "Tickable.h"
#include "UObject/WeakInterfacePtr.h"

#include "LoadingScreenManager.generated.h"

template <typename InterfaceType> class TScriptInterface;

class FSubsystemCollectionBase;
class IInputProcessor;
class ILoadingProcessInterface;
class SWidget;
class UObject;
class UWorld;
struct FFrame;
struct FWorldContext;

/**
 * Handles showing/hiding the loading screen
 */
UCLASS()
class COMMONLOADINGSCREEN_API ULoadingScreenManager : public UGameInstanceSubsystem, public FTickableGameObject
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

	UFUNCTION(BlueprintCallable, Category=LoadingScreen)
	FString GetDebugReasonForShowingOrHidingLoadingScreen() const
	{
		return DebugReasonForShowingOrHidingLoadingScreen;
	}

	/** Returns True when the loading screen is currently being shown */
	bool GetLoadingScreenDisplayStatus() const
	{
		return bCurrentlyShowingLoadingScreen;
	}

	/** Called when the loading screen visibility changes  */
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnLoadingScreenVisibilityChangedDelegate, bool);
	FORCEINLINE FOnLoadingScreenVisibilityChangedDelegate& OnLoadingScreenVisibilityChangedDelegate() { return LoadingScreenVisibilityChanged; }

	void RegisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface);
	void UnregisterLoadingProcessor(TScriptInterface<ILoadingProcessInterface> Interface);
	
private:
	
	void HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName);
	void HandlePostLoadMap(UWorld* World);

	/** Determines if we should show or hide the loading screen. Called every frame. */
	void UpdateLoadingScreen();

	/** Returns true if we need to be showing the loading screen. */
	bool CheckForAnyNeedToShowLoadingScreen();

	/** 如果我们要显示加载屏幕（如果我们确实需要这么做，或者出于其他人为原因而强制显示的话），则返回真值。 */
	bool ShouldShowLoadingScreen();

	/** 如果此时我们正处于初始加载流程中（即在使用此屏幕之前），则返回真值。 */
	bool IsShowingInitialLoadingScreen() const;

	/** 显示加载屏幕。在视口上设置加载屏幕小部件。*/
	void ShowLoadingScreen();

	/** Hides the loading screen. The loading screen widget will be destroyed */
	void HideLoadingScreen();

	/** Removes the widget from the viewport */
	void RemoveWidgetFromViewport();

	/** Prevents input from being used in-game while the loading screen is visible */
	void StartBlockingInput();

	/** Resumes in-game input, if blocking */
	void StopBlockingInput();

	void ChangePerformanceSettings(bool bEnabingLoadingScreen);

private:
	/** 当加载屏幕的可见性发生变化时触发广播通知*/
	FOnLoadingScreenVisibilityChangedDelegate LoadingScreenVisibilityChanged;

	/**我们正在显示的加载屏幕小部件的引用（如果有的话） */
	TSharedPtr<SWidget> LoadingScreenWidget;

	/** Input processor to eat all input while the loading screen is shown */
	TSharedPtr<IInputProcessor> InputPreProcessor;

	/** External loading processors, components maybe actors that delay the loading. */
	TArray<TWeakInterfacePtr<ILoadingProcessInterface>> ExternalLoadingProcessors;

	/** The reason why the loading screen is up (or not) */
	FString DebugReasonForShowingOrHidingLoadingScreen;

	/** The time when we started showing the loading screen */
	double TimeLoadingScreenShown = 0.0;

	/** The time the loading screen most recently wanted to be dismissed (might still be up due to a min display duration requirement) **/
	double TimeLoadingScreenLastDismissed = -1.0;

	/** 距离下一次出现加载屏幕提示信息的时间还有多久（即加载界面仍未消失的时间） */
	double TimeUntilNextLogHeartbeatSeconds = 0.0;

	/** 当我们位于 PreLoadMap 和 PostLoadMap 之间时为 True。 */
	bool bCurrentlyInLoadMap = false;

	/** True when the loading screen is currently being shown */
	bool bCurrentlyShowingLoadingScreen = false;
};
