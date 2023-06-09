﻿// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SViewport.h"


#include "AdvancedPreviewScene.h"
#include "SCommonEditorViewportToolbarBase.h"
#include "SEditorViewport.h"

class SEditorViewport;
class FMapViewportClient;
class FAssetEditorToolkit;
class FMapPreviewScene2D;

struct FSimpleAssetViewportRequiredArgs
{
	FSimpleAssetViewportRequiredArgs(const TSharedRef<FMapPreviewScene2D>& InPreviewScene, TSharedRef<FAssetEditorToolkit> InAssetEditorToolkit, int32 InViewportIndex)
	: PreviewScene(InPreviewScene)
	, AssetEditorToolkit(InAssetEditorToolkit)
	, ViewportIndex(InViewportIndex)
	{
	}
	
	TSharedRef<FMapPreviewScene2D> PreviewScene;
	TSharedRef<FAssetEditorToolkit> AssetEditorToolkit;
	int32 ViewportIndex;
};


/**
 * 
 */
class PROCEDURALLEVELGENERATOREDITOR_API SMapViewport : public SEditorViewport, public FGCObject, public ICommonEditorViewportToolbarInfoProvider
{
	
public:

	SLATE_BEGIN_ARGS(SMapViewport) {}
	SLATE_END_ARGS()

	/** The scene for this viewport. */
	TSharedPtr<FAdvancedPreviewScene> PreviewScene;

	void Construct(const FArguments& InArgs);
	
	SMapViewport();
	~SMapViewport();

	void AddReferencedObjects(FReferenceCollector& Collector) override;
	virtual TSharedRef<SEditorViewport> GetViewportWidget() override;
	virtual TSharedPtr<FExtender> GetExtenders() const override;
	virtual void OnFloatingButtonClicked() override;

	virtual TSharedRef<FEditorViewportClient> MakeEditorViewportClient() override;

	TSharedPtr<FMapViewportClient> GetViewportClient() { return TypedViewportClient; };
	
	//Shared ptr to the client
	TSharedPtr<FMapViewportClient> TypedViewportClient;
};
//
// struct FLinearColorArray;
//
// class PROCEDURALLEVELGENERATOREDITOR_API SMapViewport : public SViewport
// {
//
// public:
// 	SLATE_BEGIN_ARGS(SMapViewport) {}
// 	SLATE_END_ARGS()
//
// 	void Construct(const FArguments& InArgs, TArray<FLinearColorArray> MapColors);
// 	
// 	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
//     
// 	void Paint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const;
// };
