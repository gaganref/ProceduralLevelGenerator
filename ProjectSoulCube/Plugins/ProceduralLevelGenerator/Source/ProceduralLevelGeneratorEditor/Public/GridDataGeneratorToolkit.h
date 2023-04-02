﻿// Fill out your copyright notice in the Description page of Project Settings.

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

	virtual void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	virtual void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	virtual FName GetToolkitFName() const override { return "GridDataGeneratorEditor"; }
	virtual FText GetBaseToolkitName() const override { return INVTEXT("GridDataGenerator Editor"); }
	virtual FString GetWorldCentricTabPrefix() const override { return "GridDataGenerator "; }
	virtual FLinearColor GetWorldCentricTabColorScale() const override { return {}; }
	
	void GenerateGridData();
	
	int32 GetRows() const;

	int32 GetColumns() const;
	
	TArray<uint8> GetNoiseColorsRaw() const;
	
	TArray<FLinearColorArray> GetNoiseColors() const;
	
	TArray<uint8> GetMapColorsRaw() const;
	
	TArray<FLinearColorArray> GetMapColors() const;
	
private:
	
	TObjectPtr<UGridDataGenerator> GridDataGenerator;
	
	TSharedPtr<FMapPreviewScene2D> PreviewScene;
	
	TSharedPtr<SMapViewport> PreviewWidget;

	// UI action that will be bound to the toolbar button
	TSharedPtr<FUICommandInfo> GenerateCommand;

	// Create the toolbar button
	// void AddToolbarButton(FToolBarBuilder& ToolbarBuilder);
};
