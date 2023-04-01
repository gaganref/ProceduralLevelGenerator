// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid/GridDataGenerator.h"

class FMapPreviewScene2D;
class SMapViewport;
/**
 * 
 */
class PROCEDURALLEVELGENERATOREDITOR_API FGridDataGeneratorToolkit : public FAssetEditorToolkit
{
	
public:
	
	void InitEditor(const EToolkitMode::Type Mode, const TSharedPtr<IToolkitHost>& InitToolkitHost, TObjectPtr<UGridDataGenerator> InGridDataGenerator);

	void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	FName GetToolkitFName() const override { return "GridDataGeneratorEditor"; }
	FText GetBaseToolkitName() const override { return INVTEXT("GridDataGenerator Editor"); }
	FString GetWorldCentricTabPrefix() const override { return "GridDataGenerator "; }
	FLinearColor GetWorldCentricTabColorScale() const override { return {}; }

	TSharedRef<SDockTab> SpawnTab_Viewport(const FSpawnTabArgs& Args) const;

	TArray<FLinearColorArray> GetNoiseColors() const;
	
private:
	
	TObjectPtr<UGridDataGenerator> GridDataGenerator;
	TSharedPtr<FMapPreviewScene2D> PreviewScene;
	TSharedPtr<SMapViewport> PreviewWidget;
};
