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
}