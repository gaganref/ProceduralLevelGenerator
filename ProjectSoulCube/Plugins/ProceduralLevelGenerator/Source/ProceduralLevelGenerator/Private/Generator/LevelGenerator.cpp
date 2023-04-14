// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator/LevelGenerator.h"

#include "Components/FixedScaleSceneComponent.h"
#include "Misc/CustomProceduralMeshComponent.h"

// static const TArray<FLinearColor> EmptyVertexColors;

// Sets default values
ALevelGenerator::ALevelGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<UFixedScaleSceneComponent>(TEXT("SceneComponent"));
	RootComponent = SceneComponent;

	ProceduralMeshComponent = CreateDefaultSubobject<UCustomProceduralMeshComponent>(TEXT("Procedural Mesh Component"));
	ProceduralMeshComponent->SetupAttachment(RootComponent);

	SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	
}

// Called when the game starts or when spawned
void ALevelGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void ALevelGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ALevelGenerator::OnConstruction(const FTransform& Transform)
{
	Super::OnConstruction(Transform);
	SetActorScale3D(FVector(1.0f, 1.0f, 1.0f));
	
	if(bGenerateOnConstruction)
	{
		GenerateMesh();
	}

}

void ALevelGenerator::GenerateLevel() const
{
	const FDateTime StartTime = FDateTime::UtcNow();
	
	GenerateMesh();
	
	const float TimeElapsedInMs = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	const float TimeElapsedInS = (FDateTime::UtcNow() - StartTime).GetTotalSeconds();
	UE_LOG(LogTemp, Error, TEXT("generateMesh LG - Excecution time = %f MilliSeconds = %f Seconds."), TimeElapsedInMs, TimeElapsedInS);
}

void ALevelGenerator::GenerateMesh() const
{
	if(!GridData)
	{
		return;
	}

	ProceduralMeshComponent->CreateMesh(GridData);
}
