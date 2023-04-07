// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/CustomProceduralMeshComponent.h"

#include "KismetProceduralMeshLibrary.h"
#include "Misc/Structs.h"


// Sets default values for this component's properties
UCustomProceduralMeshComponent::UCustomProceduralMeshComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...

	SetRelativeScale3D(FVector(1.0f, 1.0f, 1.0f));
	UProceduralMeshComponent::SetCollisionProfileName("BlockAll");
}

void UCustomProceduralMeshComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags,
	ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	UE_LOG(LogTemp, Warning, TEXT("Works"));
	if(Scale != GetRelativeScale3D())
	{
		SetRelativeScale3D(Scale);
	}
}

void UCustomProceduralMeshComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const static FName ScaleName("RelativeScale3D");
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : FName();

	if(PropertyName == ScaleName)
	{
		SetRelativeScale3D(Scale);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}
