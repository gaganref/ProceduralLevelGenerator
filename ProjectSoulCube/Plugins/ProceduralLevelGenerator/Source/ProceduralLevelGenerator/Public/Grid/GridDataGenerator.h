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
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 235))
	int32 Rows = 100;

	// Total no of columns(↑) or total no of cells(■) in each column(↑) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1, ClampMax = 235))
	int32 Columns = 100;

	// Size of each individual cell (as a cell is a square SizeOnX = SizeOnY).
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 GridCellSize = 10;
	
	UPROPERTY(Category = "Noise Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.00001f))
	float Scale = 23.0f;

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
	FRuntimeFloatCurve HeightMultiplierCurve;

	UPROPERTY(Category = "Mesh Data", EditAnywhere, BlueprintReadOnly)
	bool bUseFlatShading;
	
	UPROPERTY(Category = "Mesh Data", VisibleAnywhere, BlueprintReadOnly)
	int32 NoiseColorsSize;

	UPROPERTY(Category = "Mesh Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MapColorsSize;

	UPROPERTY(Category = "Mesh Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MeshVertexSize;

	UPROPERTY(Category = "Mesh Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MeshUvsSize;
	
	UPROPERTY(Category = "Mesh Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MeshTriangleSize;

	UPROPERTY(Category = "Mesh Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MeshTangentsSize;

	UPROPERTY(Category = "Mesh Data", VisibleAnywhere, BlueprintReadOnly)
	int32 MeshNormalsSize;


protected:
	
	UPROPERTY()
	TArray<FLinearColorArray> NoiseColors;

	UPROPERTY()
	TArray<FLinearColorArray> MapColors;

	UPROPERTY()
	TArray<FVector> Vertices;
	
	UPROPERTY()
	TArray<FLinearColor> VertexColors;

	UPROPERTY()
	TArray<FVector2D> Uvs;

	UPROPERTY()
	TArray<int32> Triangles;

	UPROPERTY()
	TArray<FVector> Normals;

	UPROPERTY()
	TArray<struct FProcMeshTangent> Tangents;
	
protected:

	/**
	 * Called to initialize grid.
	 */
	virtual void InitGrid();

	// virtual void ClearData();

	virtual void InitColorsOnly();

		
#if WITH_EDITOR
	
	/**
	 * Called when a property on this object has been modified externally.
	 */
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;

	/**
	 * This alternate version of PostEditChange is called when properties inside structs are modified.  The property that was actually modified
	 * is located at the tail of the list.  The head of the list of the FStructProperty member variable that contains the property that was modified.
	 */
	virtual void PostEditChangeChainProperty( struct FPropertyChangedChainEvent& PropertyChangedEvent ) override;
	
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

	FORCEINLINE TArray<FTerrainType> GetLevelRegions() const {return LevelRegions;}

	FORCEINLINE float GetMeshHeightMultiplier() const {return MeshHeightMultiplier;}

	FORCEINLINE TArray<FLinearColorArray> GetNoiseColors() const {return NoiseColors;}

	FORCEINLINE TArray<FLinearColorArray> GetMapColors() const {return MapColors;}

	FORCEINLINE TArray<FVector> GetVertices() const {return Vertices;}

	FORCEINLINE TArray<FLinearColor> GetVertexColors() const {return VertexColors;}

	FORCEINLINE TArray<FVector2D> GetUvs() const {return Uvs;}

	FORCEINLINE TArray<int32> GetTriangles() const {return Triangles;}

	FORCEINLINE TArray<FVector> GetNormals() const {return Normals;}

	FORCEINLINE TArray<FProcMeshTangent> GetTangents() const {return Tangents;}

	UFUNCTION(CallInEditor)
	void GenerateData();
};
