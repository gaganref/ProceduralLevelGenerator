// Fill out your copyright notice in the Description page of Project Settings.


#include "Components/FixedScaleSceneComponent.h"

FVector UFixedScaleSceneComponent::Scale(FVector::OneVector);

UFixedScaleSceneComponent::UFixedScaleSceneComponent()
{
	SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
}

#if WITH_EDITOR
void UFixedScaleSceneComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const static FName ScaleName("RelativeScale3D");
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : FName();

	if(PropertyName == ScaleName)
	{
		SetRelativeScale3D(Scale);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
#endif

void UFixedScaleSceneComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	if (GetRelativeScale3D() != Scale)
	{
		SetRelativeScale3D(Scale);
	}
}
