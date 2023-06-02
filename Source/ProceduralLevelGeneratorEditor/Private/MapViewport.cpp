// Fill out your copyright notice in the Description page of Project Settings.


#include "MapViewport.h"

#include "MapViewportClient.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Misc/Structs.h"

//Just create the advanced preview scene and initiate components
SMapViewport::SMapViewport() : PreviewScene(MakeShareable(new FAdvancedPreviewScene(FPreviewScene::ConstructionValues())))
{
}

SMapViewport::~SMapViewport()
{
	if (TypedViewportClient.IsValid())
	{
		TypedViewportClient->Viewport = nullptr;
	}
}

void SMapViewport::Construct(const FArguments& InArgs)
{
	SEditorViewport::Construct(SEditorViewport::FArguments());
}

void SMapViewport::AddReferencedObjects(FReferenceCollector& Collector)
{
}

TSharedRef<SEditorViewport> SMapViewport::GetViewportWidget()
{
	return SharedThis(this);
}

TSharedPtr<FExtender> SMapViewport::GetExtenders() const
{
	TSharedPtr<FExtender> Result(MakeShareable(new FExtender));
	return Result;
}

void SMapViewport::OnFloatingButtonClicked()
{
}

TSharedRef<FEditorViewportClient> SMapViewport::MakeEditorViewportClient()
{
	TypedViewportClient = MakeShareable(new FMapViewportClient(SharedThis(this), PreviewScene.ToSharedRef()));
	return TypedViewportClient.ToSharedRef();
}

// void SMapViewport::Construct(const FArguments& InArgs, TArray<FLinearColorArray> MapColors)
// {
// 	struct FColorBuffer
// 	{
// 		FLinearColor Colors[100000];
// 	};
//
// 	FColorBuffer ColorBufferData;
//
// 	int32 MaxX = MapColors.Num();
// 	int32 MaxY = 0;
// 	if(MaxX > 0)
// 	{
// 		MaxY = MapColors[0].Num();
// 	}
//
// 	int32 Itr = 0;
// 	
// 	for(int X=0; X < MaxX; ++X)
// 	{
// 		for(int Y=0; Y < MaxY; ++Y)
// 		{
// 			ColorBufferData.Colors[Itr] = MapColors[X][Y];
// 			Itr++;
// 		}
// 		Itr++;
// 	}
// 	
// 	TUniformBufferRef<FColorBuffer> UniformBuffer = TUniformBufferRef<FColorBuffer>::CreateUniformBufferImmediate(ColorBufferData, EUniformBufferUsage::UniformBuffer_SingleFrame);
//
// 	UMaterial* CustomMaterial = LoadObject<UMaterial>(nullptr, TEXT("/Script/Engine.Material'/ProceduralLevelGenerator/Assets/StaticMeshes/M_PerformantShader.M_PerformantShader'"));
//
// 	UMaterialInstanceDynamic* MaterialInstance = UMaterialInstanceDynamic::Create(CustomMaterial, nullptr);
// 	FLinearColor* Colors = (FLinearColor*)UniformBuffer->Lock(LOCK_READ_ONLY);
// 	MaterialInstance->SetVectorParameterValue(TEXT("ColorBuffer"), *Colors, FLinearColor::StaticStruct());
// 	UniformBuffer->Unlock();
//
// 	FSlateBrush Brush;
// 	Brush.SetResourceObject(MaterialInstance);
// 	Brush.ImageSize = FVector2D(MaxX, MaxY);
//
// 	// Render the brush using Slate
// 	// SlateDrawBuffer.DrawBox(Position, Size, Brush, ESlateDrawEffect::None, TAttribute<FSlateColor>());
// }
//
// int32 SMapViewport::OnPaint(const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect,
// 	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
// 	bool bParentEnabled) const
// {
// 	// SViewport::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
//
// 	Paint(Args, AllottedGeometry, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
//     
// 	return SViewport::OnPaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled);
// }
//
// void SMapViewport::Paint(const FPaintArgs& Args, const FGeometry& AllottedGeometry,
// 	FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle,
// 	bool bParentEnabled) const
// {
// }
