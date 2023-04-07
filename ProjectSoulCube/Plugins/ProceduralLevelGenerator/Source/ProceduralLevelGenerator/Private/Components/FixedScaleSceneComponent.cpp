// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FixedScaleSceneComponent.h"

UFixedScaleSceneComponent::UFixedScaleSceneComponent()
{
	SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
}

void UFixedScaleSceneComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const static FName ScaleName("RelativeScale3D");
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : FName();

	if(PropertyName == ScaleName)
	{
		SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UFixedScaleSceneComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	UE_LOG(LogTemp, Warning, TEXT("Works"));
	// If the actor's scale is different from 1.0f, reset it to 1.0f
	
	if (GetRelativeScale3D() != FVector(1.0f, 1.0f, 1.0f))
	{
		SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	}
}
