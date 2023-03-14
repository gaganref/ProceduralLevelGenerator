// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator/LevelGenerator.h"

#include "Kismet/KismetMathLibrary.h"
#include "Misc/Noise.h"

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


// Sets default values
ALevelGenerator::ALevelGenerator()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	SceneComponent = CreateDefaultSubobject<USceneComponent>(TEXT("SceneComponent"));

	// Make the scene component the root component
	RootComponent = SceneComponent;

	
#if WITH_EDITORONLY_DATA
	
	NoisePlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Noise Plane"));
	NoisePlane->SetupAttachment(SceneComponent);
	NoisePlane->SetVisibility(bShowNoisePlane);
	
	MapPlane = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Map Plane"));
	MapPlane->SetupAttachment(SceneComponent);
	MapPlane->SetVisibility(bShowMapPlane);
#endif	
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

void ALevelGenerator::PostActorCreated()
{
	Super::PostActorCreated();

	InitLevelGenerator();
}

void ALevelGenerator::PostLoad()
{
	Super::PostLoad();

	InitLevelGenerator();
}

void ALevelGenerator::InitLevelGenerator()
{
	const TArray<FFloatArray>NoiseMap = UNoise::GenerateNoiseMap(Rows,Columns, Scale);
	GenerateMapsOnPlanes(NoiseMap);
}

void ALevelGenerator::GenerateMapsOnPlanes(const TArray<FFloatArray>& NoiseMap)
{
	TObjectPtr<UTexture2D> NoiseTexture = GenerateTextureFromNoise(NoiseMap);
	
}

TObjectPtr<UTexture2D> ALevelGenerator::GenerateTextureFromNoise(const TArray<FFloatArray>& NoiseMap)
{
	const int32 MapX = NoiseMap[0].Num();
	const int32 MapY = NoiseMap.Num();
	TObjectPtr<UTexture2D> Texture = UTexture2D::CreateTransient(MapX, MapY);


	// @Ref: https://www.orfeasel.com/generating-procedural-textures/

	//uint8* Pixels = GeneratePixels(TextureHeight, TextureWidth);
	//Each element of the array contains a single color so in order to represent information in
	//RGBA we need to have an array of length TextureWidth * TextureHeight * 4
	uint8* Pixels = new uint8[MapX * MapY * 4];
	for(int Y=0; Y < MapY; Y++)
	{
		for(int X=0; X < MapX; X++)
		{
			//Get the current pixel
			const int32 CurrentPixelIndex = ((Y * MapX) + X);

			// lerp the colour to generate smooth noise gradient
			FLinearColor NoiseLinearColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, NoiseMap[X][Y]);
			FColor NoiseColor = NoiseLinearColor.ToFColor(true);

			// DEBUG_PRINT_CUSTOM_TEXT(NoiseColor.ToString());
			// DEBUG_PRINT_CUSTOM_TEXT(NoiseLinearColor.ToString());
			
			Pixels[4 * CurrentPixelIndex] = NoiseColor.B; //b
			Pixels[4 * CurrentPixelIndex + 1] = NoiseColor.G; //g
			Pixels[4 * CurrentPixelIndex + 2] = NoiseColor.R; //r
			Pixels[4 * CurrentPixelIndex + 3] = 255; //set A channel always to maximum
		}
	}

	//Allocate first mipmap.
	FTexture2DMipMap* Mip = new FTexture2DMipMap();
	Texture->GetPlatformData()->Mips.Add(Mip);
	Mip->SizeX = MapX;
	Mip->SizeY = MapY;

	//Lock the mipmap data so it can be modified
	Mip->BulkData.Lock(LOCK_READ_WRITE);
	uint8* TextureData = (uint8*)Mip->BulkData.Realloc(MapX * MapY * 4);
	//Copy the pixel data into the Texture data
	FMemory::Memcpy(TextureData, Pixels, sizeof(uint8) * MapX * MapY * 4);
	Mip->BulkData.Unlock();
 
 
	//Initialize a new texture
	Texture->Source.Init(MapX, MapY, 1, 1, ETextureSourceFormat::TSF_BGRA8, Pixels);
	Texture->UpdateResource();

#if WITH_EDITORONLY_DATA	
	// Assign texture to the material
	UMaterialInstanceDynamic* DynamicMaterial = NoisePlane->CreateDynamicMaterialInstance(0, NoisePlane->GetMaterial(0));
	DynamicMaterial->SetTextureParameterValue("Texture", Texture);
	NoisePlane->SetMaterial(0, DynamicMaterial);
#endif
	//Since we don't need access to the pixel data anymore free the memory
	delete[] Pixels;
	
	return Texture;
}
