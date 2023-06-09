﻿// Fill out your copyright notice in the Description page of Project Settings.


#include "GridDataGeneratorToolkit.h"

#include "MapPreviewScene2D.h"
#include "MapViewport.h"
#include "Widgets/SMapTextureViewport.h"

#define LOCTEXT_NAMESPACE "GridDataGenerator"

void FGridDataGeneratorToolkit::InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, TObjectPtr<UGridDataGenerator> InGridDataGenerator)
{
	GridDataGenerator = Cast<UGridDataGenerator>(InGridDataGenerator);
	
	// Create Preview Scene
	if (!PreviewScene.IsValid())
	{
		PreviewScene = MakeShareable(
			new FMapPreviewScene2D(
				FPreviewScene::ConstructionValues()
				.AllowAudioPlayback(true)
				.ShouldSimulatePhysics(true)
				.ForceUseMovementComponentInNonGameWorld(true),
				StaticCastSharedRef<FGridDataGeneratorToolkit>(AsShared())));
	}
	
	// Create viewport widget
	PreviewWidget = SNew(SMapViewport);
	
	const TSharedRef<FTabManager::FLayout> Layout = FTabManager::NewLayout("GridDataGeneratorEditorLayout")
	->AddArea
	(
		FTabManager::NewPrimaryArea()->SetOrientation(Orient_Vertical)
		->Split
		(
			FTabManager::NewSplitter()
			->SetSizeCoefficient(1.0f)
			->SetOrientation(Orient_Horizontal)
			->Split
			(
				FTabManager::NewSplitter()->SetOrientation(Orient_Vertical)
				->SetSizeCoefficient(0.7f)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
					->SetSizeCoefficient(0.7f)
					->Split
					(
					FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab("NoiseMapTab", ETabState::OpenedTab)
					)
					->Split
					(
					FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab("ColorMapTab", ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.3f)
					->AddTab("OutputLog", ETabState::OpenedTab)
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.3f)
				->AddTab("GridDataGeneratorDetailsTab", ETabState::OpenedTab)
			)
		)
	);
	
	InitAssetEditor(EToolkitMode::Standalone, {}, "GridDataGeneratorEditor", Layout, true, true, InGridDataGenerator);
}


void FGridDataGeneratorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("GridDataGenerator Editor"));

	//Register Tabs

	InTabManager->RegisterTabSpawner("NoiseMapTab", FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		[
		SNew(SMapTextureViewport)
		.Width(this, &FGridDataGeneratorToolkit::GetRows)
		.Height(this, &FGridDataGeneratorToolkit::GetColumns)
		.ColorsRaw(this, &FGridDataGeneratorToolkit::GetNoiseColorsRaw)
		];
	}))
	.SetDisplayName(INVTEXT("Noise Map"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef())
	.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelViewport.Icon"));
	
	InTabManager->RegisterTabSpawner("ColorMapTab", FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		[
			SNew(SMapTextureViewport)
			.Width(this, &FGridDataGeneratorToolkit::GetRows)
			.Height(this, &FGridDataGeneratorToolkit::GetColumns)
			.ColorsRaw(this, &FGridDataGeneratorToolkit::GetMapColorsRaw)
		];
	}))
	.SetDisplayName(INVTEXT("Color Map"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef())
	.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Levels"));
	
	FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
	
	FDetailsViewArgs DetailsViewArgs;
	DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
	
	TSharedRef<IDetailsView> DetailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
	DetailsView->SetObjects(TArray<UObject*>{ GridDataGenerator });
	
	InTabManager->RegisterTabSpawner("GridDataGeneratorDetailsTab", FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		[
			DetailsView
		];
	}))
	.SetDisplayName(INVTEXT("Properties")) // Grid Data
	.SetGroup(WorkspaceMenuCategory.ToSharedRef())
	.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "LevelEditor.Tabs.Properties")); //DataTableEditor.Paste.Small
}

void FGridDataGeneratorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner("NoiseMapTab");
	InTabManager->UnregisterTabSpawner("ColorMapTab");
	InTabManager->UnregisterTabSpawner("GridDataGeneratorDetailsTab");
}

int32 FGridDataGeneratorToolkit::GetRows() const
{
	return GridDataGenerator->GetRows();
}

int32 FGridDataGeneratorToolkit::GetColumns() const
{
	return GridDataGenerator->GetColumns();
}

TArray<uint8> FGridDataGeneratorToolkit::GetNoiseColorsRaw() const
{
	return GridDataGenerator->GetNoiseColorsRaw();
}

TArray<uint8> FGridDataGeneratorToolkit::GetMapColorsRaw() const
{
	return GridDataGenerator->GetMapColorsRaw();
}

#undef LOCTEXT_NAMESPACE
