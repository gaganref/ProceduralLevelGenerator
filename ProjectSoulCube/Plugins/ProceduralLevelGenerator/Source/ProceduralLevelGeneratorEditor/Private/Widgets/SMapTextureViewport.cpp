// Fill out your copyright notice in the Description page of Project Settings.


#include "SMapTextureViewport.h"

#include "Editor.h"
#include "Misc/Structs.h"


void SMapTextureViewport::Construct(const FArguments& InArgs)
{
	NoiseColors = InArgs._NoiseColors;

	PixelBrush = new FSlateImageBrush
	(
		TEXT(""),
		FVector2D(32, 32),
		FSlateColor(FLinearColor::Red),
		ESlateBrushTileType::NoTile,
		ESlateBrushImageType::NoImage
	);
	PixelBrush->Margin = FMargin(0);
	PixelBrush->DrawAs = ESlateBrushDrawType::Image;
}

int32 SMapTextureViewport::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	int32 MaxX = NoiseColors.Get().Num();
	int32 MaxY = 0;
	if(MaxX > 0)
	{
		MaxY = NoiseColors.Get()[0].Num();
	}
	
	FVector2d Offset = FVector2D(4.0f, 4.0f);
	FVector2D LocalSize = FVector2D(1.0f, 1.0f);
	
	for(int X=0; X < MaxX; ++X)
	{
		Offset.Y = 4.0f;
		for(int Y=0; Y < MaxY; ++Y)
		{
			FPaintGeometry PaintGeom = AllottedGeometry.ToPaintGeometry(Offset, LocalSize, 5.0f);
			FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeom, PixelBrush, ESlateDrawEffect::None, NoiseColors.Get()[X][Y]);
			Offset.Y = Offset.Y + LocalSize.Y;
		}
		Offset.X = Offset.X + LocalSize.X;
	}
	return LayerId;
}

FVector2D SMapTextureViewport::ComputeDesiredSize(float) const
{
	return FVector2D(200.0, 200.0);
}

FReply SMapTextureViewport::OnMouseButtonDown(const FGeometry& AllottedGeometry, const FPointerEvent& MouseEvent)
{
	if (GEditor && GEditor->CanTransact() && ensure(!GIsTransacting))
		GEditor->BeginTransaction(TEXT(""), INVTEXT("Edit Normal Distribution"), nullptr);
	return FReply::Handled().CaptureMouse(SharedThis(this));
}

FReply SMapTextureViewport::OnMouseButtonUp(const FGeometry& AllottedGeometry, const FPointerEvent& MouseEvent)
{
	if (GEditor) GEditor->EndTransaction();
	return FReply::Handled().ReleaseMouseCapture();
}

FReply SMapTextureViewport::OnMouseMove(const FGeometry& AllottedGeometry, const FPointerEvent& MouseEvent)
{
	if (!HasMouseCapture()) return FReply::Unhandled();
	const FTransform2D PointsTransform = GetPointsTransform(AllottedGeometry);
	const FVector2D LocalPosition = AllottedGeometry.AbsoluteToLocal(MouseEvent.GetScreenSpacePosition());
	const FVector2D NormalizedPosition = PointsTransform.Inverse().TransformPoint(LocalPosition);
	return FReply::Handled();
}

FTransform2D SMapTextureViewport::GetPointsTransform(const FGeometry& AllottedGeometry) const
{
	const double Margin = 0.05 * AllottedGeometry.GetLocalSize().GetMin();
	const FScale2D Scale((AllottedGeometry.GetLocalSize() - 2.0 * Margin) * FVector2D(1.0, -1.0));
	const FVector2D Translation(Margin, AllottedGeometry.GetLocalSize().Y - Margin);
	return FTransform2D(Scale, Translation);
}
