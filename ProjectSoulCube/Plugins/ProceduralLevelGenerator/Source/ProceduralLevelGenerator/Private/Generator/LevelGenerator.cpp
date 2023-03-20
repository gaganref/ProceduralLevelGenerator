// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator/LevelGenerator.h"

#include "Kismet/KismetMathLibrary.h"
#include "Misc/CustomProceduralMeshComponent.h"
#include "Misc/DynamicTextureComponent.h"
#include "Misc/Noise.h"
#include "Misc/Structs.h"

#define DEBUG_GET_CURR_CLASS_FUNC (FString(__FUNCTION__))
#define DEBUG_GET_CURR_LINE (FString::FromInt(__LINE__))

#define DEBUG_LOG_SIMPLE UE_LOG(LogTemp, Error, TEXT("Debug- Simple log."))
#define DEBUG_MESSAGE_SIMPLE if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Debug- Simple message.")))
#define DEBUG_PRINT_SIMPLE DEBUG_LOG_SIMPLE DEBUG_MESSAGE_SIMPLE

#define DEBUG_LOG_SIMPLE_WITH_INFO UE_LOG(LogTemp, Error, TEXT("Debug- Simple log with info. || Called at- %s::%s."), *DEBUG_GET_CURR_CLASS_FUNC, *DEBUG_GET_CURR_LINE)
#define DEBUG_MESSAGE_SIMPLE_WITH_INFO if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Debug- Simple message with info. || Called at- %s::%s."), *DEBUG_GET_CURR_CLASS_FUNC, *DEBUG_GET_CURR_LINE))
#define DEBUG_PRINT_SIMPLE_WITH_INFO DEBUG_LOG_SIMPLE_WITH_INFO DEBUG_MESSAGE_SIMPLE_WITH_INFO

#define DEBUG_LOG_CUSTOM_TEXT(text) UE_LOG(LogTemp, Error, TEXT("Debug- %s"), *FString(text))
#define DEBUG_MESSAGE_CUSTOM_TEXT(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Debug - %s"), *FString(text)))
#define DEBUG_PRINT_CUSTOM_TEXT(text) DEBUG_LOG_CUSTOM_TEXT(text) DEBUG_MESSAGE_CUSTOM_TEXT(text)

#define DEBUG_LOG_CUSTOM_TEXT_WITH_INFO(text) UE_LOG(LogTemp, Error, TEXT("Debug- %s || Called at- %s::%s."), *FString(text), *DEBUG_GET_CURR_CLASS_FUNC, *DEBUG_GET_CURR_LINE)
#define DEBUG_MESSAGE_CUSTOM_TEXT_WITH_INFO(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Debug - %s || Called at- %s::%s."), *FString(text), *DEBUG_GET_CURR_CLASS_FUNC, *DEBUG_GET_CURR_LINE))
#define DEBUG_PRINT_CUSTOM_TEXT_WITH_INFO(text) DEBUG_LOG_CUSTOM_TEXT_WITH_INFO(text) DEBUG_MESSAGE_CUSTOM_TEXT_WITH_INFO(text)

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
	const TArray<FFloatArray>& NoiseMap = UNoise::GenerateNoiseMap(Seed, Rows, Columns, Scale, Octaves, Persistence, Lacunarity, Offset);

	if(bShowNoisePlane)
	{
		GenerateTextureOnNoisePlane(NoiseMap);
	}
	if(bShowMapPlane)
	{
		GenerateTextureOnMapPlane(NoiseMap);
	}
	
}

void ALevelGenerator::GenerateTextureOnNoisePlane(const TArray<FFloatArray>& NoiseMap)
{	
	const int32 MapX = NoiseMap[0].Num();
	const int32 MapY = NoiseMap.Num();
	
	TArray<FLinearColorArray> LinearColorArray;
	
	for(int Y=0; Y < MapY; ++Y)
	{
		FLinearColorArray LinearColors;
		for(int X=0; X < MapX; ++X)
		{
			FLinearColor NoiseLinearColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, NoiseMap[Y][X]);
			LinearColors.Add(NoiseLinearColor);
		}
		LinearColorArray.Add(LinearColors);
	}

	NoiseTextureComponent->ReInitialize(Rows, Columns, FLinearColor::White, LinearColorArray);
	NoiseTextureComponent->UpdateTexture();
	const TObjectPtr<UTexture2D> NoiseTexture = NoiseTextureComponent->GetTextureResource();
	
	// Assign texture to the material
	UMaterialInstanceDynamic* DynamicMaterial = NoisePlane->CreateDynamicMaterialInstance(0, NoisePlane->GetMaterial(0));
	DynamicMaterial->SetTextureParameterValue("Texture", NoiseTexture);
	NoisePlane->SetMaterial(0, DynamicMaterial);
}

void ALevelGenerator::GenerateTextureOnMapPlane(const TArray<FFloatArray>& NoiseMap)
{
	const int32 MapX = NoiseMap[0].Num();
	const int32 MapY = NoiseMap.Num();
	
	TArray<FLinearColorArray> LinearColorArray;
	
	for(int Y=0; Y < MapY; ++Y)
	{
		FLinearColorArray LinearColors;
		for(int X=0; X < MapX; ++X)
		{
			const float CurrentHeight = NoiseMap[Y][X];
			FLinearColor RegionColor = FLinearColor::Red;
			FString Temp = FString("TempColor");
			
			for(const auto& Region : LevelRegions)
			{
				if(CurrentHeight <= Region.MaxHeight)
				{
					Temp = Region.TerrainName;
					RegionColor = Region.LinearColor;
					break;
				}
			}
			LinearColors.Add(RegionColor);
		}
		LinearColorArray.Add(LinearColors);
	}

	MapTextureComponent->ReInitialize(Rows, Columns, FLinearColor::White, LinearColorArray);
	MapTextureComponent->UpdateTexture();
	const TObjectPtr<UTexture2D> NoiseTexture = MapTextureComponent->GetTextureResource();
	
	// Assign texture to the material
	UMaterialInstanceDynamic* DynamicMaterial = MapPlane->CreateDynamicMaterialInstance(0, MapPlane->GetMaterial(0));
	DynamicMaterial->SetTextureParameterValue("Texture", NoiseTexture);
	MapPlane->SetMaterial(0, DynamicMaterial);

	if(bShowMesh)
	{
		ProceduralMeshComponent->GenerateTerrainMesh(NoiseMap, MeshHeightMultiplier);
		ProceduralMeshComponent->SetMaterial(0, DynamicMaterial);
	}
}
