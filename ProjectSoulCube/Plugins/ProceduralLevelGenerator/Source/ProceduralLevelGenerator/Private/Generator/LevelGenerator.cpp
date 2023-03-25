// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator/LevelGenerator.h"

#include "Grid/Grid.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/CustomProceduralMeshComponent.h"
#include "Misc/DynamicTextureComponent.h"
#include "Misc/Noise.h"
#include "Misc/Structs.h"

static TArray<FLinearColorArray> PixelColorArrayEmpty;

// Sets default values
ALevelGenerator::ALevelGenerator(const FObjectInitializer& ObjectInitializer)
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// Make the scene component the root component
	RootComponent = SceneComponent;

#if WITH_EDITORONLY_DATA
	NoisePlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Noise Plane"));
	NoisePlane->SetupAttachment(SceneComponent);
	
	MapPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map Plane"));
	MapPlane->SetupAttachment(SceneComponent);
#endif

	NoiseTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Noise Texture Component"));
	NoiseTextureComponent->Initialize(Rows, Columns, FLinearColor::White, PixelColorArrayEmpty);

	MapTextureComponent = CreateDefaultSubobject<UDynamicTextureComponent>(TEXT("Map Texture Component"));
	MapTextureComponent->Initialize(Rows, Columns, FLinearColor::White, PixelColorArrayEmpty);

	ProceduralMeshComponent = CreateDefaultSubobject<UCustomProceduralMeshComponent>(TEXT("Procedural Mesh Component"));
	ProceduralMeshComponent->SetCollisionProfileName("BlockAll");
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

	if(bGenerateNoisePlaneOnConstruction)
	{
		InitLevelGenerator();
	}
}

void ALevelGenerator::GenerateMap()
{
	InitLevelGenerator();
}

void ALevelGenerator::InitLevelGenerator()
{
	if(bShowNoisePlane)
	{
		GenerateTextureOnNoisePlane();
	}
	if(bShowMapPlane)
	{
		GenerateTextureOnMapPlane();
	}
}

void ALevelGenerator::GenerateTextureOnNoisePlane()
{
	if(!GridData)
	{
		return;
	}
	
	NoiseTextureComponent->ReInitialize(GridData->GetRows(), GridData->GetColumns(), FLinearColor::White, GridData->GetNoiseColors());
	NoiseTextureComponent->UpdateTexture();
	const TObjectPtr<UTexture2D> NoiseTexture = NoiseTextureComponent->GetTextureResource();
	
	// Assign texture to the material
	UMaterialInstanceDynamic* DynamicMaterial = NoisePlane->CreateDynamicMaterialInstance(0, NoisePlane->GetMaterial(0));
	DynamicMaterial->SetTextureParameterValue("Texture", NoiseTexture);
	NoisePlane->SetMaterial(0, DynamicMaterial);
}

void ALevelGenerator::GenerateTextureOnMapPlane()
{
	if(!GridData)
	{
		return;
	}
	
	MapTextureComponent->ReInitialize(GridData->GetRows(), GridData->GetColumns(), FLinearColor::White, GridData->GetMapColors());
	MapTextureComponent->UpdateTexture();
	const TObjectPtr<UTexture2D> NoiseTexture = MapTextureComponent->GetTextureResource();
	
	// Assign texture to the material
	UMaterialInstanceDynamic* DynamicMaterial = MapPlane->CreateDynamicMaterialInstance(0, MapPlane->GetMaterial(0));
	DynamicMaterial->SetTextureParameterValue("Texture", NoiseTexture);
	MapPlane->SetMaterial(0, DynamicMaterial);

	if(bShowMesh)
	{
		const FCustomMeshData& MeshData = GridData->GetMeshData();
		// ProceduralMeshComponent->CreateMeshSection_LinearColor(0, MeshData.Vertices, MeshData.Triangles, MeshData.Normals, MeshData.Uvs, MeshData.VertexColors, MeshData.Tangents, true);
		
		ProceduralMeshComponent->CreateMeshSection_LinearColor(0, GridData->Vertices, GridData->Triangles, GridData->Normals, GridData->Uvs, GridData->VertexColors, GridData->Tangents, true);

		ProceduralMeshComponent->SetMaterial(0, DynamicMaterial);
	}
}
