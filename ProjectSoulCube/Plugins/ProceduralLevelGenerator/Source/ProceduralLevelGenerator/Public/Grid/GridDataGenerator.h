// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Generator/GeneratorHelpers.h"
#include "UObject/Object.h"
#include "Misc/Structs.h"
#include "GridDataGenerator.generated.h"

enum EFallOffShape;
class FDisjointSet;
class UQuadTree;
struct FLinearColorArray;
struct FTerrainType;
class UCurveFloat;

/**
 * 
 */
UCLASS(BlueprintType)
class PROCEDURALLEVELGENERATOR_API UGridDataGenerator : public UObject
{
	GENERATED_BODY()

public:

	UGridDataGenerator();
	
protected:

	// Random Seed value
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly)
	int32 Seed = 252;
	
	// Total no of rows(→) or total no of cells(■) in each row(→) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 235, Delta = 5))
	int32 Rows = 100;

	// Total no of columns(↑) or total no of cells(■) in each column(↑) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 235, Delta = 5))
	int32 Columns = 100;
	
	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.00001f, Delta = 1))
	float Scale = 30.0f;

	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 10, Delta = 1))
	int32 Octaves = 5;

	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1, Delta = 0.01))
	float Persistence = 0.5f;

	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 10, Delta = 0.01))
	float Lacunarity = 2.0f;

	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (AllowPreserveRatio, Delta = 1))
	FVector2D Offset;

	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (TitleProperty="TerrainName"))
	TArray<FTerrainType> LevelRegions;

	UPROPERTY(Category = "FallOff Data", EditAnywhere, BlueprintReadOnly)
	TEnumAsByte<EFallOffShape> FallOffShape = Fos_Square;
	
	UPROPERTY(Category = "FallOff Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1, Delta = 0.001))
	float FallOffStart = 0.8f;

	UPROPERTY(Category = "FallOff Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1, Delta = 0.001))
	float FallOffEnd = 1.0f;

	UPROPERTY(Category = "FallOff Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 4, Delta = 0.001))
	float FallOffPower = 2.0f;

	UPROPERTY(Category = "FallOff Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1, Delta = 0.001))
	float FallOffInfluence = 1.0f;
	
	UPROPERTY(Category = "Mesh Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1.0 , AllowPreserveRatio, Delta = 1))
	FVector MeshScale = FVector(100.0f, 100.0f, 100.0f);
	
	UPROPERTY(Category = "Mesh Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 100, Delta = 0.5, SliderExponent = 1))
	float MeshHeightMultiplier = 30.0f;
	
	UPROPERTY(Category = "Mesh Data", EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveFloat> HeightMultiplierCurve;

	
protected:

	UPROPERTY()
	TArray<float> NoiseMap;
	
	UPROPERTY()
	TArray<uint8> NoiseColorsRaw;

	UPROPERTY()
	TArray<uint8> MapColorsRaw;

protected:
	
	virtual void InitGridColorData();
	
	virtual void GenerateGridColorData();
		
#if WITH_EDITOR
	
	/**
	 * Called when a property on this object has been modified externally.
	 */
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif

public:

	UFUNCTION(Category = "Level Data", CallInEditor)
	void SortLevelRegions();
	
	UFUNCTION(BlueprintCallable)
	void RefreshNoiseMap();
	
	UFUNCTION(BlueprintCallable)
	float GetGridWidth() const;

	UFUNCTION(BlueprintCallable)
	float GetGridHeight() const;

	UFUNCTION(BlueprintCallable)
	TArray<bool> GetGridPathInfo() const;

	UFUNCTION(BlueprintCallable)
	void CalculateRegionData(TArray<uint8>& OutRegionIndexMapping, TArray<int32>& OutRegionCellCount);

	float GetCellNoiseAveraged(const int32 InX, const int32 InY) const;
	
public:

	// Getters
	
	FORCEINLINE int32 GetRows() const {return Rows;}
	
	FORCEINLINE int32 GetColumns() const {return Columns;}

	FORCEINLINE int32 GetSeed() const {return Seed;}

	FORCEINLINE float GetScale() const {return Scale;}

	FORCEINLINE int32 GetOctaves() const {return Octaves;}

	FORCEINLINE float GetPersistence() const {return Persistence;}

	FORCEINLINE float GetLacunarity() const {return Lacunarity;}

	FORCEINLINE FVector2D GetOffset() const {return Offset;}

	FORCEINLINE const TArray<FTerrainType>& GetLevelRegions() const {return LevelRegions;}

	FORCEINLINE float GetMeshHeightMultiplier() const {return MeshHeightMultiplier;}

	FORCEINLINE const TArray<uint8>& GetNoiseColorsRaw() const {return NoiseColorsRaw;}

	FORCEINLINE const TArray<uint8>& GetMapColorsRaw() const {return MapColorsRaw;}

	FORCEINLINE FVector GetMeshScale() const {return MeshScale;}

	FORCEINLINE TArray<float> GetCurrentNoiseMap() const;
	
	FORCEINLINE TArray<float> GetCurrentNoiseMapNormalized() const;
	
};
