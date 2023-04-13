// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Misc/Structs.h"
#include "GridDataGenerator.generated.h"

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

	UPROPERTY(Category = "Mesh Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1.0 , AllowPreserveRatio, Delta = 1))
	FVector MeshScale = FVector(100.0f, 100.0f, 100.0f);
	
	UPROPERTY(Category = "Mesh Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 100, Delta = 0.5, SliderExponent = 1))
	float MeshHeightMultiplier = 30.0f;
	
	UPROPERTY(Category = "Mesh Data", EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveFloat> HeightMultiplierCurve;

	
protected:

	UPROPERTY()
	TArray<uint8> NoiseColorsRaw;

	UPROPERTY()
	TArray<uint8> MapColorsRaw;

	UPROPERTY()
	TArray<uint8> RegionIndexMapping;

	UPROPERTY()
	TArray<int32> RegionCellCount;

	// to store if the is valid such that it is in a valid region or in a valid height.
	UPROPERTY()
	TArray<bool> IsValidCell;

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

	/**
	 *  GeneratePoisonDiskPoints inspired from From https://github.com/corporateshark/poisson-disk-generator/blob/master/PoissonGenerator.h
	 */

	// To generate points that are apart from each other a minimum distance.
	UFUNCTION(BlueprintCallable)
	TArray<FVector2D> GeneratePoisonDiskPoints(int32 NoOfPoints, float MinimumDistance, int32 NoOfTries);

	// To generate points that are apart from each other a minimum distance and evenly (using poison alg).
	UFUNCTION(BlueprintCallable)
	TArray<FVector2D> GeneratePoisonDiskPointsEvenly(float MinimumDistance, int32 MaxPoints);
	
	void FloodFill(FDisjointSet& OutDisjointSet);

	UFUNCTION(BlueprintCallable)
	TArray<FVector2D> FindConnectedPoints2D(const FVector2D& InputPoint, const TArray<FVector2D>& TargetPoints);

	UFUNCTION(BlueprintCallable)
	TArray<FVector> FindConnectedPoints(const FVector& InputLocation, const TArray<FVector>& TargetLocations);

	UFUNCTION(BlueprintCallable)
	float GetGridWidth() const;

	UFUNCTION(BlueprintCallable)
	float GetGridHeight() const;

	UFUNCTION(BlueprintCallable)
	TArray<bool> GetGridPathInfo() const;

	FVector2D GenerateRandomPointAround(const FVector2D& Point, const float& MinimumDistance, const FRandomStream& RandomStream) const;
	
	FORCEINLINE int32 GetCellIndex(const int32& GridX, const int32& GridY) const;

	FORCEINLINE FVector2D GetCellIndex2D(const int32& GridIndex) const;

	FORCEINLINE int32 GetCellIndex(const FVector2D& GridXY) const;
	
	FORCEINLINE int32 GetCellIndexByLocation(const FVector& Location) const;

	FORCEINLINE int32 GetCellIndexByLocation(const FVector2D& Location) const;

	FORCEINLINE FVector2D GetCellIndex2DByLocation(const FVector2D& Location) const;

	FORCEINLINE bool IsPointInGrid(const FVector2D& Point) const;

	FORCEINLINE bool IsPointInCell(const FVector2D& Point, const FVector2D& CellPosition) const;
	
	UFUNCTION(Category = "Level Data", CallInEditor)
	void SortLevelRegions();
	
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

	FORCEINLINE TArray<FFloatArray> GetCurrentNoiseMap() const;
	
	FORCEINLINE TArray<FFloatArray> GetCurrentNoiseMapNormalized() const;

	FORCEINLINE int32 GetRegionCellCount(const int32 SectionIndex) const;

	FORCEINLINE int32 GetRegionIndex(const int32 GridX, const int32 GridY) const;

	void CalculateRegionData(TArray<uint8>& OutRegionIndexMapping, TArray<int32>& OutRegionCellCount);
};
