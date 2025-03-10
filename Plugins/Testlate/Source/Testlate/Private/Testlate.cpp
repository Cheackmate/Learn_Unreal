// Copyright Epic Games, Inc. All Rights Reserved.

#include "Testlate.h"
#include "TestlateStyle.h"
#include "TestlateCommands.h"
#include "LevelEditor.h"
#include "Widgets/Docking/SDockTab.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Text/STextBlock.h"
#include "ToolMenus.h"

static const FName TestlateTabName("Testlate");

#define LOCTEXT_NAMESPACE "FTestlateModule"

void FTestlateModule::StartupModule()
{
	// This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
	
	FTestlateStyle::Initialize();
	FTestlateStyle::ReloadTextures();

	FTestlateCommands::Register();
	
	PluginCommands = MakeShareable(new FUICommandList);

	PluginCommands->MapAction(
		FTestlateCommands::Get().OpenPluginWindow,
		FExecuteAction::CreateRaw(this, &FTestlateModule::PluginButtonClicked),
		FCanExecuteAction());

	UToolMenus::RegisterStartupCallback(FSimpleMulticastDelegate::FDelegate::CreateRaw(this, &FTestlateModule::RegisterMenus));
	
	FGlobalTabmanager::Get()->RegisterNomadTabSpawner(TestlateTabName, FOnSpawnTab::CreateRaw(this, &FTestlateModule::OnSpawnPluginTab))
		.SetDisplayName(LOCTEXT("FTestlateTabTitle", "Testlate"))
		.SetMenuType(ETabSpawnerMenuType::Hidden);
}

void FTestlateModule::ShutdownModule()
{
	// This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
	// we call this function before unloading the module.

	UToolMenus::UnRegisterStartupCallback(this);

	UToolMenus::UnregisterOwner(this);

	FTestlateStyle::Shutdown();

	FTestlateCommands::Unregister();

	FGlobalTabmanager::Get()->UnregisterNomadTabSpawner(TestlateTabName);
}

TSharedRef<SDockTab> FTestlateModule::OnSpawnPluginTab(const FSpawnTabArgs& SpawnTabArgs)
{
	FText WidgetText = FText::Format(
		LOCTEXT("WindowWidgetText", "Add code to {0} in {1} to override this window's contents"),
		FText::FromString(TEXT("FTestlateModule::OnSpawnPluginTab")),
		FText::FromString(TEXT("Testlate.cpp"))
		);

	return SNew(SDockTab)
		.TabRole(ETabRole::NomadTab)
		[
			// Put your tab content here!
			SNew(SBox)
			.HAlign(HAlign_Center)
			.VAlign(VAlign_Center)
			[
				SNew(STextBlock)
				.Text(WidgetText)
			]
		];
}

void FTestlateModule::PluginButtonClicked()
{
	FGlobalTabmanager::Get()->TryInvokeTab(TestlateTabName);
}

void FTestlateModule::RegisterMenus()
{
	// Owner will be used for cleanup in call to UToolMenus::UnregisterOwner
	FToolMenuOwnerScoped OwnerScoped(this);

	{
		UToolMenu* Menu = UToolMenus::Get()->ExtendMenu("LevelEditor.MainMenu.Window");
		{
			FToolMenuSection& Section = Menu->FindOrAddSection("WindowLayout");
			Section.AddMenuEntryWithCommandList(FTestlateCommands::Get().OpenPluginWindow, PluginCommands);
		}
	}

	{
		UToolMenu* ToolbarMenu = UToolMenus::Get()->ExtendMenu("LevelEditor.LevelEditorToolBar");
		{
			FToolMenuSection& Section = ToolbarMenu->FindOrAddSection("Settings");
			{
				FToolMenuEntry& Entry = Section.AddEntry(FToolMenuEntry::InitToolBarButton(FTestlateCommands::Get().OpenPluginWindow));
				Entry.SetCommandList(PluginCommands);
			}
		}
	}
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FTestlateModule, Testlate)