// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator/TreesGenerator.h"

#include "Components/InstancedStaticMeshComponent.h"
#include "Kismet/KismetMathLibrary.h"


// Sets default values
ATreesGenerator::ATreesGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	SimpleTree = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("Simple Tree"));
	SimpleTree->SetupAttachment(SceneComponent);

	XmasTree = CreateDefaultSubobject<UInstancedStaticMeshComponent>(TEXT("XmasTree Tree"));
	XmasTree->SetupAttachment(SceneComponent);
}

// Called when the game starts or when spawned
void ATreesGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ATreesGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ATreesGenerator::GenerateTrees() const
{
	FRandomStream RandomStream;

	
	// for(int Itr = 0; Itr < 10 < Itr++)
	// {
	// 	SimpleTree->AddInstance()
	// }
}
