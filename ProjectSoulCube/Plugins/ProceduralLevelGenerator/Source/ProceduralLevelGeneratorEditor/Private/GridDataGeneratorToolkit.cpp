// Fill out your copyright notice in the Description page of Project Settings.


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
				->SetSizeCoefficient(0.75f)
				->Split
				(
					FTabManager::NewSplitter()->SetOrientation(Orient_Horizontal)
					->Split
					(
					FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab("GridDataGeneratorViewportTab", ETabState::OpenedTab)
					)
					->Split
					(
					FTabManager::NewStack()
						->SetSizeCoefficient(0.5f)
						->AddTab("NoiseMapTab", ETabState::OpenedTab)
					)
				)
				->Split
				(
					FTabManager::NewStack()
					->SetSizeCoefficient(0.35f)
					->AddTab("OutputLog", ETabState::OpenedTab)
				)
			)
			->Split
			(
				FTabManager::NewStack()
				->SetSizeCoefficient(0.25f)
				->AddTab("GridDataGeneratorDetailsTab", ETabState::OpenedTab)
			)
		)
	);
	
	InitAssetEditor(EToolkitMode::Standalone, {}, "GridDataGeneratorEditor", Layout, true, true, InGridDataGenerator);
}

TSharedRef<SDockTab> FGridDataGeneratorToolkit::SpawnTab_Viewport(const FSpawnTabArgs& Args) const
{
	TSharedRef<SDockTab> SpawnedTab = SNew(SDockTab).Label(LOCTEXT("ViewportTab_Title", "Viewport"));

	if (PreviewWidget.IsValid())
	{
		SpawnedTab->SetContent(PreviewWidget.ToSharedRef());
	}

	return SpawnedTab;
}

void FGridDataGeneratorToolkit::RegisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(InTabManager);

	WorkspaceMenuCategory = InTabManager->AddLocalWorkspaceMenuCategory(INVTEXT("GridDataGenerator Editor"));

	//Register Viewport
	InTabManager->RegisterTabSpawner(
	"GridDataGeneratorViewportTab",
	FOnSpawnTab::CreateSP(this, &FGridDataGeneratorToolkit::SpawnTab_Viewport))
	.SetDisplayName(LOCTEXT("PreviewSceneViewport", "Preview Viewport"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef())
	.SetIcon(FSlateIcon(FAppStyle::GetAppStyleSetName(), "GraphEditor.EventGraph_16x"));

	InTabManager->RegisterTabSpawner("NoiseMapTab", FOnSpawnTab::CreateLambda([=](const FSpawnTabArgs&)
	{
		return SNew(SDockTab)
		[
			SNew(SMapTextureViewport)
			.NoiseColors(this, &FGridDataGeneratorToolkit::GetNoiseColors)
		];
	}))
	.SetDisplayName(INVTEXT("Noise Map"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());
	
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
	.SetDisplayName(INVTEXT("Details"))
	.SetGroup(WorkspaceMenuCategory.ToSharedRef());
}

void FGridDataGeneratorToolkit::UnregisterTabSpawners(const TSharedRef<FTabManager>& InTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(InTabManager);
	InTabManager->UnregisterTabSpawner("GridDataGeneratorViewportTab");
	InTabManager->UnregisterTabSpawner("GridDataGeneratorDetailsTab");
	InTabManager->UnregisterTabSpawner("NoiseMapTab");
}

TArray<FLinearColorArray> FGridDataGeneratorToolkit::GetNoiseColors() const
{
	return GridDataGenerator->GetNoiseColors();
}

#undef LOCTEXT_NAMESPACE
