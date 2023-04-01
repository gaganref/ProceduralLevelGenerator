// Fill out your copyright notice in the Description page of Project Settings.


#include "MapPreviewScene2D.h"

#include "GameFramework/WorldSettings.h"


FMapPreviewScene2D::FMapPreviewScene2D(ConstructionValues Cvs, const TSharedRef<FGridDataGeneratorToolkit>& EditorToolkit)
	: FAdvancedPreviewScene(Cvs), EditorPtr(EditorToolkit)
{
	// Disable killing actors outside of the world
	AWorldSettings* WorldSettings = GetWorld()->GetWorldSettings(true);
	WorldSettings->bEnableWorldBoundsChecks = false;
}

FMapPreviewScene2D::~FMapPreviewScene2D()
{
}

void FMapPreviewScene2D::Tick(float InDeltaTime)
{
	FAdvancedPreviewScene::Tick(InDeltaTime);
}
