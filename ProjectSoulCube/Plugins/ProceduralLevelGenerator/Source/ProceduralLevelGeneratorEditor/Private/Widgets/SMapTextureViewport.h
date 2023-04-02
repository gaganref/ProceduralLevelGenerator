// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Widgets/SLeafWidget.h"

struct FLinearColorArray;

/**
 * 
 */
class PROCEDURALLEVELGENERATOREDITOR_API SMapTextureViewport : public SLeafWidget
{
	
public:
	
	SLATE_BEGIN_ARGS(SMapTextureViewport)
	{}
	SLATE_ATTRIBUTE(int32, Width)
	SLATE_ATTRIBUTE(int32, Height)
	SLATE_ATTRIBUTE(TArray<uint8>, ColorsRaw)
SLATE_END_ARGS()

	
	void Construct(const FArguments& InArgs);

	virtual int32 OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const override;
	virtual FVector2D ComputeDesiredSize(float) const override;
	
private:

	TAttribute<int32> Width;

	TAttribute<int32> Height;

	TAttribute<TArray<uint8>> ColorsRaw;

	TObjectPtr<FSlateBrush> TextureBrush;
};
