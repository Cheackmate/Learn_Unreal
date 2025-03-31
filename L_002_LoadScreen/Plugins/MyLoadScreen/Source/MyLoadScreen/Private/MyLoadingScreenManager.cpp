// Fill out your copyright notice in the Description page of Project Settings.


#include "MyLoadingScreenManager.h"

#include <wingdi.h>
#include "Blueprint/UserWidget.h"
#include "MyLoadingScreenSettings.h"
#include "PreLoadScreen.h"
#include "PreLoadScreenManager.h"
#include "Framework/Application/IInputProcessor.h"
#include "HAL/ThreadHeartBeat.h"
#include "Widgets/Images/SThrobber.h"


//////////////////////////////////////////////////////////////////////
// FLoadingScreenInputPreProcessor

// Input processor to throw in when loading screen is shown
// This will capture any inputs, so active menus under the loading screen will not interact
class FLoadingScreenInputPreProcessor : public IInputProcessor
{
public:
	FLoadingScreenInputPreProcessor() { }
	virtual ~FLoadingScreenInputPreProcessor() { }

	bool CanEatInput() const
	{
		return !GIsEditor;
	}

	//~IInputProcess interface
	virtual void Tick(const float DeltaTime, FSlateApplication& SlateApp, TSharedRef<ICursor> Cursor) override { }

	virtual bool HandleKeyDownEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanEatInput(); }
	virtual bool HandleKeyUpEvent(FSlateApplication& SlateApp, const FKeyEvent& InKeyEvent) override { return CanEatInput(); }
	virtual bool HandleAnalogInputEvent(FSlateApplication& SlateApp, const FAnalogInputEvent& InAnalogInputEvent) override { return CanEatInput(); }
	virtual bool HandleMouseMoveEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonDownEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonUpEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseButtonDoubleClickEvent(FSlateApplication& SlateApp, const FPointerEvent& MouseEvent) override { return CanEatInput(); }
	virtual bool HandleMouseWheelOrGestureEvent(FSlateApplication& SlateApp, const FPointerEvent& InWheelEvent, const FPointerEvent* InGestureEvent) override { return CanEatInput(); }
	virtual bool HandleMotionDetectedEvent(FSlateApplication& SlateApp, const FMotionEvent& MotionEvent) override { return CanEatInput(); }
	//~End of IInputProcess interface
};

void UMyLoadingScreenManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	// 在这里绑定地图加载事件
	FCoreUObjectDelegates::PreLoadMapWithContext.AddUObject(this, &ThisClass::HandlePreLoadMap);
	FCoreUObjectDelegates::PostLoadMapWithWorld.AddUObject(this, &ThisClass::HandlePostLoadMap);
	const UGameInstance* LocalGameInstance = GetGameInstance();
	check(LocalGameInstance);
}

void UMyLoadingScreenManager::Deinitialize()
{
	StopBlockingInput();

	RemoveWidgetFromViewport();
	
	FCoreUObjectDelegates::PreLoadMap.RemoveAll(this);
	FCoreUObjectDelegates::PostLoadMapWithWorld.RemoveAll(this);
	
	Super::Deinitialize();
}

bool UMyLoadingScreenManager::ShouldCreateSubsystem(UObject* Outer) const
{
	// Only clients have loading screens
	const UGameInstance* GameInstance = CastChecked<UGameInstance>(Outer);
	const bool bIsServerWorld = GameInstance->IsDedicatedServerInstance();	
	return !bIsServerWorld;
}

void UMyLoadingScreenManager::Tick(float DeltaTime)
{
	UpdateLoadingScreen();
}

ETickableTickType UMyLoadingScreenManager::GetTickableTickType() const
{
	return FTickableGameObject::GetTickableTickType();
}

bool UMyLoadingScreenManager::IsTickable() const
{
	return FTickableGameObject::IsTickable();
}

TStatId UMyLoadingScreenManager::GetStatId() const
{
	RETURN_QUICK_DECLARE_CYCLE_STAT(ULoadingScreenManager, STATGROUP_Tickables);
}

UWorld* UMyLoadingScreenManager::GetTickableGameObjectWorld() const
{
	return  GetGameInstance()->GetWorld();
}

void UMyLoadingScreenManager::RegisterLoadingProcessor(UMyLoadingProecessTask* Interface)
{
	ExternalLoadingProcessors.Add(Interface);
}

void UMyLoadingScreenManager::UnregisterLoadingProcessor(UMyLoadingProecessTask* Interface)
{
	ExternalLoadingProcessors.Remove(Interface);
}

void UMyLoadingScreenManager::UpdateLoadingScreen()
{
	if (ShouldShowLoadingScreen())
	{
		const UMyLoadingScreenSettings* Settings = GetDefault<UMyLoadingScreenSettings>();
		
		ShowLoadingScreen();
		
	}
	else
	{
		HideLoadingScreen();
 
		FThreadHeartBeat::Get().MonitorCheckpointEnd(GetFName());
	}
	
}

inline void UMyLoadingScreenManager::HandlePreLoadMap(const FWorldContext& WorldContext, const FString& MapName)
{
	if (WorldContext.OwningGameInstance == GetGameInstance())
	{
		bCurrentlyInLoadMap = true;

		// Update the loading screen immediately if the engine is initialized
		if (GEngine->IsInitialized())
		{
			UpdateLoadingScreen();
		}
	}
}

inline void UMyLoadingScreenManager::HandlePostLoadMap(UWorld* World)
{
	if ((World != nullptr) && (World->GetGameInstance() == GetGameInstance()))
	{
		bCurrentlyInLoadMap = false;
	}
}

void UMyLoadingScreenManager::RemoveWidgetFromViewport()
{
	UGameInstance* LocalGameInstance = GetGameInstance();
	if (LoadingScreenWidget.IsValid())
	{
		if (UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient())
		{
			GameViewportClient->RemoveViewportWidgetContent(LoadingScreenWidget.ToSharedRef());
		}
		LoadingScreenWidget.Reset();
	}
}

void UMyLoadingScreenManager::StartBlockingInput()
{
	if (!InputPreProcessor.IsValid())
	{
		InputPreProcessor = MakeShareable<FLoadingScreenInputPreProcessor>(new FLoadingScreenInputPreProcessor());
		FSlateApplication::Get().RegisterInputPreProcessor(InputPreProcessor, 0);
	}
}

void UMyLoadingScreenManager::StopBlockingInput()
{
	if (InputPreProcessor.IsValid())
	{
		FSlateApplication::Get().UnregisterInputPreProcessor(InputPreProcessor);
		InputPreProcessor.Reset();
	}
}

bool UMyLoadingScreenManager::ShouldShowLoadingScreen()
{
	
	// 1.导入在项目界面配置好的插件信息
	const UMyLoadingScreenSettings* Settings = GetDefault<UMyLoadingScreenSettings>();

	// Can't show a loading screen if there's no game viewport
	UGameInstance* LocalGameInstance = GetGameInstance();
	if (LocalGameInstance->GetGameViewportClient() == nullptr)
	{
		return false;
	}

	// Check for a need to show the loading screen
	const bool bNeedToShowLoadingScreen = CheckForAnyNeedToShowLoadingScreen();

	// 如果需要，将加载屏幕保持更长的时间 （？这里直接写死是不想展示了么？行吧）
	bool bWantToForceShowLoadingScreen = false;

	return bNeedToShowLoadingScreen || bWantToForceShowLoadingScreen;
}

bool UMyLoadingScreenManager::CheckForAnyNeedToShowLoadingScreen()
{
	
	// Start out with 'unknown' reason in case someone forgets to put a reason when changing this in the future.
	DebugReasonForShowingOrHidingLoadingScreen = TEXT("Reason for Showing/Hiding LoadingScreen is unknown!");

	const UGameInstance* LocalGameInstance = GetGameInstance();


	const FWorldContext* Context = LocalGameInstance->GetWorldContext();
	if (Context == nullptr)
	{
		// We don't have a world context right now... better show a loading screen
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("The game instance has a null WorldContext"));
		return true;
	}

	UWorld* World = Context->World();
	if (World == nullptr)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("We have no world (FWorldContext's World() is null)"));
		return true;
	}

	AGameStateBase* GameState = World->GetGameState<AGameStateBase>();
	if (GameState == nullptr)
	{
		// The game state has not yet replicated.
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("GameState hasn't yet replicated (it's null)"));
		return true;
	}

	if (bCurrentlyInLoadMap)
	{
		// Show a loading screen if we are in LoadMap
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("bCurrentlyInLoadMap is true"));
		return true;
	}

	if (!Context->TravelURL.IsEmpty())
	{
		// Show a loading screen when pending travel
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("We have pending travel (the TravelURL is not empty)"));
		return true;
	}

	if (Context->PendingNetGame != nullptr)
	{
		// Connecting to another server
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("We are connecting to another server (PendingNetGame != nullptr)"));
		return true;
	}

	if (!World->HasBegunPlay())
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("World hasn't begun play"));
		return true;
	}

	if (World->IsInSeamlessTravel())
	{
		// Show a loading screen during seamless travel
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("We are in seamless travel"));
		return true;
	}
	

	// Check each local player
	bool bFoundAnyLocalPC = false;
	bool bMissingAnyLocalPC = false;

	for (ULocalPlayer* LP : LocalGameInstance->GetLocalPlayers())
	{
		if (LP != nullptr)
		{
			if (APlayerController* PC = LP->PlayerController)
			{
				bFoundAnyLocalPC = true;
			}
			else
			{
				bMissingAnyLocalPC = true;
			}
		}
	}

	UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();
	const bool bIsInSplitscreen = GameViewportClient->GetCurrentSplitscreenConfiguration() != ESplitScreenType::None;

	// In splitscreen we need all player controllers to be present
	if (bIsInSplitscreen && bMissingAnyLocalPC)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("At least one missing local player controller in splitscreen"));
		return true;
	}

	// And in non-splitscreen we need at least one player controller to be present
	if (!bIsInSplitscreen && !bFoundAnyLocalPC)
	{
		DebugReasonForShowingOrHidingLoadingScreen = FString(TEXT("Need at least one local player controller"));
		return true;
	}

	// Victory! The loading screen can go away now
	DebugReasonForShowingOrHidingLoadingScreen = TEXT("(nothing wants to show it anymore)");
	return false;
}

void UMyLoadingScreenManager::ShowLoadingScreen()
{
	
	// 如果正在加载屏幕则不进行操作。
	if (bCurrentlyShowingLoadingScreen)
	{
		return;
	}

	// 开始加载屏幕的时间
	TimeLoadingScreenShown = FPlatformTime::Seconds();
	// 表示当前正在加载屏幕
	bCurrentlyShowingLoadingScreen = true;

	const UMyLoadingScreenSettings* Settings = GetDefault<UMyLoadingScreenSettings>();

	// 如果此时我们正处于初始加载流程中（即在使用此屏幕之前），则不管了
	if (IsShowingInitialLoadingScreen())
	{
	}
	else
		{
		UGameInstance* LocalGameInstance = GetGameInstance();

		// Eat input while the loading screen is displayed
		// 停止接受输入
		StartBlockingInput();

		// Create the loading screen widget
		// 从配置好的插件信息中加载对应的LoadingScreenWidget
		TSubclassOf<UUserWidget> LoadingScreenWidgetClass = Settings->LoadingScreenWidget.TryLoadClass<UUserWidget>();
		if (UUserWidget* UserWidget = UUserWidget::CreateWidgetInstance(*LocalGameInstance, LoadingScreenWidgetClass, NAME_None))
		{
			// 获取slate组件，如果该组件不存在则构建它
			LoadingScreenWidget = UserWidget->TakeWidget();
		}
		else
		{
			LoadingScreenWidget = SNew(SThrobber);
		}

		// Add to the viewport at a high ZOrder to make sure it is on top of most things
		// 配置屏幕组件的展示优先级，确保在最上层
		UGameViewportClient* GameViewportClient = LocalGameInstance->GetGameViewportClient();
		// UI组件添加到窗口中
		GameViewportClient->AddViewportWidgetContent(LoadingScreenWidget.ToSharedRef(), Settings->LoadingScreenZOrder);
	}
}

void UMyLoadingScreenManager::HideLoadingScreen()
{
	
}

bool UMyLoadingScreenManager::IsShowingInitialLoadingScreen()
{
	FPreLoadScreenManager* PreLoadScreenManager = FPreLoadScreenManager::Get();
	return (PreLoadScreenManager != nullptr) && PreLoadScreenManager->HasValidActivePreLoadScreen();
}
