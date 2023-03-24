// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Misc/Structs.h"
#include "GridDataGenerator.generated.h"

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

	
protected:

	// Random Seed value
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly)
	int32 Seed = 252;
	
	// Total no of rows(→) or total no of cells(■) in each row(→) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 Rows = 100;

	// Total no of columns(↑) or total no of cells(■) in each column(↑) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 Columns = 100;

	// Size of each individual cell (as a cell is a square SizeOnX = SizeOnY)
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.00001f))
	float Scale = 23.0f;

	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
	int32 Octaves = 5;

	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1))
	float Persistence = 0.5f;

	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	float Lacunarity = 2.0f;

	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly)
	FVector2D Offset;

	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly)
	TArray<FTerrainType> LevelRegions;
	
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	float MeshHeightMultiplier = 36.0f;

	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly)
	FRuntimeFloatCurve HeightMultiplierCurve;
	
	UPROPERTY(Category = "Grid Data", VisibleAnywhere, BlueprintReadOnly)
	int32 NoiseColorsSize;

	UPROPERTY(Category = "Grid Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MapColorsSize;

	UPROPERTY(Category = "Grid Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MeshVertexSize;

	UPROPERTY(Category = "Grid Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MeshUvsSize;
	
	UPROPERTY(Category = "Grid Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MeshTriangleSize;

	UPROPERTY(Category = "Grid Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MeshTangentsSize;

	UPROPERTY(Category = "Grid Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MeshNormalsSize;
	
	UPROPERTY()
	TArray<FLinearColorArray> NoiseColors;

	UPROPERTY()
	TArray<FLinearColorArray> MapColors;

	UPROPERTY()
	TArray<FVector> Vertices;

	FCustomMeshData MeshData;
	
	// UPROPERTY()
	// TArray<FGridCell> GridCells;

protected:

	/**
	 * Called to initialize grid.
	 */
	virtual void InitGrid();

	virtual void ClearData();

public:

	// Getters
	
	FORCEINLINE int32 GetRows() const {return Rows;}
	
	FORCEINLINE int32 GetColumns() const {return Columns;}

	FORCEINLINE int32 GetSeed() const {return Seed;}

	FORCEINLINE int32 GetRows1() const {return Rows;}

	FORCEINLINE int32 GetColumns1() const {return Columns;}

	FORCEINLINE float GetScale() const {return Scale;}

	FORCEINLINE int32 GetOctaves() const {return Octaves;}

	FORCEINLINE float GetPersistence() const {return Persistence;}

	FORCEINLINE float GetLacunarity() const {return Lacunarity;}

	FORCEINLINE FVector2D GetOffset() const {return Offset;}

	FORCEINLINE TArray<FTerrainType> GetLevelRegions() const {return LevelRegions;}

	FORCEINLINE float GetMeshHeightMultiplier() const {return MeshHeightMultiplier;}

	// FORCEINLINE TObjectPtr<UCurveFloat> GetHeightMultiplierCurve() const {return HeightMultiplierCurve;}

	FORCEINLINE TArray<FLinearColorArray> GetNoiseColors() const {return NoiseColors;}

	FORCEINLINE TArray<FLinearColorArray> GetMapColors() const {return MapColors;}

	FORCEINLINE const FCustomMeshData& GetMeshData() const {return MeshData;}

	// FORCEINLINE TArray<FGridCell> GetGridCells() const {return GridCells;}

	UFUNCTION(CallInEditor)
	void GenerateData();
};
