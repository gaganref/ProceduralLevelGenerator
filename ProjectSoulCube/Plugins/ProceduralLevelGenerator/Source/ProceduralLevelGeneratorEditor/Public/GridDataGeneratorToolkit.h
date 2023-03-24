// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Grid/GridDataGenerator.h"

/**
 * 
 */
class PROCEDURALLEVELGENERATOREDITOR_API FGridDataGeneratorToolkit : public FAssetEditorToolkit
{
	
public:
	
	void InitEditor(const TArray<UObject*>& InObjects);

	void RegisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;
	void UnregisterTabSpawners(const TSharedRef<class FTabManager>& TabManager) override;

	FName GetToolkitFName() const override { return "NormalDistributionEditor"; }
	FText GetBaseToolkitName() const override { return INVTEXT("Normal Distribution Editor"); }
	FString GetWorldCentricTabPrefix() const override { return "Normal Distribution "; }
	FLinearColor GetWorldCentricTabColorScale() const override { return {}; }

private:
	TObjectPtr<UGridDataGenerator> GridDataGenerator;
	
};
