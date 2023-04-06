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

private:
	// UPROPERTY()
	// TArray<FFloatArray> NoiseData;
	//
	// UPROPERTY()
	// TArray<FFloatArray> NoiseDataNormalized;
	
protected:

	// Random Seed value
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly)
	int32 Seed = 252;
	
	// Total no of rows(→) or total no of cells(■) in each row(→) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 235))
	int32 Rows = 100;

	// Total no of columns(↑) or total no of cells(■) in each column(↑) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 235))
	int32 Columns = 100;
	
	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.00001f))
	float Scale = 45.0f;

	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0))
	int32 Octaves = 5;

	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0, ClampMax = 1))
	float Persistence = 0.5f;

	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	float Lacunarity = 2.0f;

	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly)
	FVector2D Offset;

	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly)
	TArray<FTerrainType> LevelRegions;
	
	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	float MeshHeightMultiplier = 36.0f;
	
	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly)
	TObjectPtr<UCurveFloat> HeightMultiplierCurve;
	
protected:
	
	UPROPERTY()
	TArray<uint8> NoiseColorsRaw;
	
	UPROPERTY()
	TArray<FLinearColorArray> NoiseColors;

	UPROPERTY()
	TArray<uint8> MapColorsRaw;
	
	UPROPERTY()
	TArray<FLinearColorArray> MapColors;

	UPROPERTY()
	TArray<FCell> Cells;

	UPROPERTY()
	TArray<uint32> SectionCellCount;
	
	UPROPERTY()
	TArray<FVector> Vertices;

	UPROPERTY()
	TArray<FVector2D> Uvs;

	UPROPERTY()
	TArray<int32> Triangles;

	UPROPERTY()
	TArray<FVector> Normals;

	UPROPERTY()
	TArray<struct FProcMeshTangent> Tangents;

	UPROPERTY()
	TArray<struct FLevelSection> MeshSectionsData;

protected:

	virtual void GenerateGridData(const bool bCanGenerateMeshData);
	
	virtual void InitGridColorData();
	
	virtual void ClearGridColorData();

	virtual void InitGridMeshData();
	
	virtual void ClearGridMeshData();
	
	virtual void GenerateGridColorData();
	
	virtual void GenerateGridMeshData();	// Flat shading with tris

	virtual void GenerateGridMeshSectionsData();	// Flat shading with tris
		
#if WITH_EDITOR
	
	/**
	 * Called when a property on this object has been modified externally.
	 */
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif

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
	
	FORCEINLINE const TArray<FLinearColorArray>& GetNoiseColors() const {return NoiseColors;}

	FORCEINLINE const TArray<uint8>& GetMapColorsRaw() const {return MapColorsRaw;}
	
	FORCEINLINE const TArray<FLinearColorArray>& GetMapColors() const {return MapColors;}

	FORCEINLINE const TArray<FVector>& GetVertices() const {return Vertices;}

	FORCEINLINE const TArray<FVector2D>& GetUvs() const {return Uvs;}

	FORCEINLINE const TArray<int32>& GetTriangles() const {return Triangles;}

	FORCEINLINE const TArray<FVector>& GetNormals() const {return Normals;}

	FORCEINLINE const TArray<struct FLevelSection>& GetMeshSectionsData() const {return MeshSectionsData;}
	
	FORCEINLINE const TArray<FProcMeshTangent>& GetTangents() const {return Tangents;}
	
	UFUNCTION(CallInEditor)
	void GenerateData();
};
