// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Brushes/SlateImageBrush.h"
#include "Widgets/SLeafWidget.h"

struct FLinearColorArray;

static TArray<FLinearColorArray> EmptyLinearColorArray;

/**
 * 
 */
class PROCEDURALLEVELGENERATOREDITOR_API SMapTextureViewport : public SLeafWidget
{
public:
public:
	
	SLATE_BEGIN_ARGS(SMapTextureViewport)
		: _NoiseColors(EmptyLinearColorArray)
	{}
	SLATE_ATTRIBUTE(TArray<FLinearColorArray>, NoiseColors)
SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	FVector2D ComputeDesiredSize(float) const override;

	FReply OnMouseButtonDown(const FGeometry& AllottedGeometry, const FPointerEvent& MouseEvent) override;
	FReply OnMouseButtonUp(const FGeometry& AllottedGeometry, const FPointerEvent& MouseEvent) override;
	FReply OnMouseMove(const FGeometry& AllottedGeometry, const FPointerEvent& MouseEvent) override;
private:

	TAttribute<TArray<FLinearColorArray>> NoiseColors;
	
	FTransform2D GetPointsTransform(const FGeometry& AllottedGeometry) const;

	TObjectPtr<FSlateImageBrush> PixelBrush;
};
