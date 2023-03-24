// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

/**
 * 
 */
class PROCEDURALLEVELGENERATOREDITOR_API SMapTextureViewport : public SLeafWidget
{
public:
public:
	
	SLATE_BEGIN_ARGS(SMapTextureViewport)
		: _Mean(0.5f)
		, _StandardDeviation(0.2f)
	{}
	SLATE_ATTRIBUTE(float, Mean)
	SLATE_ATTRIBUTE(float, StandardDeviation)
SLATE_END_ARGS()

	void Construct(const FArguments& InArgs);

	int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	FVector2D ComputeDesiredSize(float) const override;

	FReply OnMouseButtonDown(const FGeometry& AllottedGeometry, const FPointerEvent& MouseEvent) override;
	FReply OnMouseButtonUp(const FGeometry& AllottedGeometry, const FPointerEvent& MouseEvent) override;
	FReply OnMouseMove(const FGeometry& AllottedGeometry, const FPointerEvent& MouseEvent) override;
private:
	TAttribute<float> Mean;
	TAttribute<float> StandardDeviation;

	FTransform2D GetPointsTransform(const FGeometry& AllottedGeometry) const;
};
