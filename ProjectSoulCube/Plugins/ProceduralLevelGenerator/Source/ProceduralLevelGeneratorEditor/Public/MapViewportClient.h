// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedPreviewScene.h"

/**
 * 
 */
class PROCEDURALLEVELGENERATOREDITOR_API FMapViewportClient : public FEditorViewportClient
{
	
public:

	/** Pointer back to the Editor Viewport */
	TWeakPtr<class SEditorViewport> ViewportPtr;

	//Constructor and destructor
	FMapViewportClient(const TSharedRef<SEditorViewport>& InThumbnailViewport, const TSharedRef<FAdvancedPreviewScene>& InPreviewScene);
	~FMapViewportClient();
	
	/** Stored pointer to the preview scene in which the static mesh is shown */
	FAdvancedPreviewScene* AdvancedPreviewScene;
};
