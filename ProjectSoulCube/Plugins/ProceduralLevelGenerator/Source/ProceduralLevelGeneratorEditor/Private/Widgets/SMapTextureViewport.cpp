// Fill out your copyright notice in the Description page of Project Settings.


#include "SMapTextureViewport.h"

#include "Editor.h"

void SMapTextureViewport::Construct(const FArguments& InArgs)
{
	Width = InArgs._Width;
	Height = InArgs._Height;
	ColorsRaw = InArgs._ColorsRaw;

	TextureBrush = new FSlateBrush();
	TextureBrush->Tiling = ESlateBrushTileType::NoTile;
	TextureBrush->DrawAs = ESlateBrushDrawType::Box;
}

int32 SMapTextureViewport::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId,
	const FWidgetStyle& InWidgetStyle, bool bParentEnabled) const
{
	if(ColorsRaw.Get().Num() < 1 || Width.Get() < 1 || Height.Get() < 1)
	{
		return LayerId;
	}
	
	// Texture Method
	
	const TObjectPtr<UTexture2D> DynamicTexture = UTexture2D::CreateTransient(Width.Get(), Height.Get(), PF_B8G8R8A8);

	FMemory::Memcpy(DynamicTexture->GetPlatformData()->Mips[0].BulkData.Lock(LOCK_READ_WRITE), ColorsRaw.Get().GetData(), ColorsRaw.Get().Num());
	DynamicTexture->GetPlatformData()->Mips[0].BulkData.Unlock();
	
	DynamicTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap;
	DynamicTexture->SRGB = true;
	DynamicTexture->UpdateResource();

	TextureBrush->SetResourceObject(DynamicTexture);
	TextureBrush->SetImageSize(FVector2D(DynamicTexture->GetSizeX(), DynamicTexture->GetSizeY()));
	
	FVector2D LocalSize = FVector2D(400.0f, 400.0f);

	const FSlateLayoutTransform LayoutTransform(1.0f, FVector2D(30.0f, 30.0f));
	
	FPaintGeometry PaintGeom = AllottedGeometry.ToPaintGeometry(LocalSize, LayoutTransform);
	FSlateDrawElement::MakeBox(OutDrawElements, LayerId, PaintGeom, TextureBrush);

	return LayerId;
}

FVector2D SMapTextureViewport::ComputeDesiredSize(float) const
{
	return FVector2D(200.0, 200.0);
}