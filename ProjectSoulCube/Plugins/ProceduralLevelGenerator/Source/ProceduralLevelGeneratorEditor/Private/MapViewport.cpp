// Fill out your copyright notice in the Description page of Project Settings.


#include "MapViewport.h"

#include "MapViewportClient.h"

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
