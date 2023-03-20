// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

struct FTerrainType;
class UDynamicTextureComponent;
struct FFloatArray;
class UCustomProceduralMeshComponent;

UCLASS()
class PROCEDURALLEVELGENERATOR_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

private:

	/** Default root scene component. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;

	/** Plane where the perlin noise is projected using texture. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> NoisePlane;

	/** Plane where the generated map is projected using texture. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MapPlane;

	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bDebug = false;

	/** Plane where the perlin noise is projected using texture. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDynamicTextureComponent> NoiseTextureComponent;

	/** Plane where the perlin noise is projected using texture. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UDynamicTextureComponent> MapTextureComponent;

	/** Plane where the perlin noise is projected using texture. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCustomProceduralMeshComponent> ProceduralMeshComponent;
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator() = default;

	ALevelGenerator(const FObjectInitializer& ObjectInitializer);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
protected:
	void InitLevelGenerator();

	void GenerateTextureOnNoisePlane(const TArray<FFloatArray>& NoiseMap);
	void GenerateTextureOnMapPlane(const TArray<FFloatArray>& NoiseMap);
	
public:

	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetNoisePlane() const {return NoisePlane;}
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetMapPlane() const {return MapPlane;}


public:
	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly)
	int32 Seed = 231;
	
	// Total no of rows(→) or total no of cells(■) in each row(→) in the grid.
	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 Rows = 10;

	// Total no of columns(↑) or total no of cells(■) in each column(↑) in the grid.
	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 Columns = 10;

	// Size of each individual cell (as a cell is a square SizeOnX = SizeOnY)
	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.00001f))
	float Scale = 0.2f;

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
	int32 Octaves = 4;

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1))
	float Persistence = 0.6f;

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	float Lacunarity = 2.3f;

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly)
	FVector2D Offset;

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	float MeshHeightMultiplier = 5.0f;

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly)
	TArray<FTerrainType> LevelRegions;

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly)
	bool bShowNoisePlane = false;

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly)
	bool bShowMapPlane = false;

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly)
	bool bShowMesh = false;
	
	// Use this with caution as changing values very quickly can crash your editor
	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly)
	bool bGenerateNoisePlaneOnConstruction = false;
	
	UFUNCTION(Category = "Level Data", BlueprintCallable, CallInEditor)
	void GenerateMap();
};
