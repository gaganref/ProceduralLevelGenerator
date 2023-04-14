// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "GeneratorHelpers.generated.h"

class FDisjointSet;
struct FFloatArray;
/**
 * 
 */

UENUM(BlueprintType)
enum EFallOffShape
{
	Fos_Circle				UMETA(DisplayName = "Circle"),
	Fos_Square				UMETA(DisplayName = "Square"),
	Fos_Diamond				UMETA(DisplayName = "Diamond")
};

UCLASS()
class PROCEDURALLEVELGENERATOR_API UGeneratorHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:
	
	static TArray<FVector2D> CalculateOcataveOffsets(const int32 Seed, const int32 Octaves, const FVector2D Offset);

	static void FloodFill(FDisjointSet& OutDisjointSet, const TArray<bool>& GridPathInfo, int32 Rows, int32 Columns);

public:

	UFUNCTION(BlueprintCallable, Category = "Grid")
	static int32 CoordinateToIndex(const int32 X, const int32 Y, const int32 Rows);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	static int32 CoordinateToIndexWithFVector2D(const FVector2D Coordinate, const int32 Rows);
	
	UFUNCTION(BlueprintCallable, Category = "Grid")
	static FVector2D IndexToCoordinate(const int32 Index, const int32 Rows);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	static int32 LocationToIndex(const FVector2D Location, const float GridWidth, const float GridHeight, const float CellWidth, const float CellHeight, const int32 Rows);
	
	UFUNCTION(BlueprintCallable, Category = "Grid")
	static FVector2D LocationToCoordinate(const FVector2D Location, const float GridWidth, const float GridHeight, const float CellWidth, const float CellHeight);

	UFUNCTION(BlueprintCallable, Category = "Grid")
	static bool IsPointInGrid(const FVector2D Point, const float Width, const float Height);

	UFUNCTION(BlueprintCallable, Category = "Noise")
	static TArray<FFloatArray> GenerateFallOffMap(const int32 MapWidth, const int32 MapHeight, const float FallOffStart, const float FallOffEnd, const float FallOffPower, const float FallOffInfluence, TEnumAsByte<EFallOffShape> FallOffShape);
	
	UFUNCTION(BlueprintCallable, Category = "Noise")
	static float CalculatePerlinValueAtPoint(const int32 MapHalfWidth, const int32 MapHalfHeight, const int32 PointX, const int32 PointY, float Scale,
	                                         const int32 Octaves, const float Persistence, const float Lacunarity, const TArray<FVector2D>& OctaveOffsets);
	
	UFUNCTION(BlueprintCallable, Category = "Noise")
	static TArray<FFloatArray> GenerateNoiseMap(const int32 Seed, const int32 MapWidth, const int32 MapHeight, float Scale, const int32 Octaves, const float
	                                            Persistence, const float Lacunarity, const FVector2D Offset);

	UFUNCTION(BlueprintCallable, Category = "Noise")
	static TArray<FFloatArray> GenerateNoiseMapNormalized(const int Seed, const int32 MapWidth, const int32 MapHeight, float Scale, const int32 Octaves, const float Persistence, const
	                                                      float Lacunarity, const FVector2D Offset, const UCurveFloat* NormalizeCurve, const float MeshHeightMultiplier, const
	                                                      float ClampMin, const float ClampMax);

	UFUNCTION(BlueprintCallable, Category = "Noise")
	static TArray<FFloatArray> NormalizeNoiseMap(const TArray<FFloatArray>& NoiseMap, const UCurveFloat* NormalizeCurve, const float MeshHeightMultiplier, const float
	                                             ClampMin = 0.0f, const float ClampMax = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "RandomPoints")
	static FVector2D GenerateInitialPoissonPoint(const FRandomStream& RandomStream, const int32 MaximumTries, const TArray<bool>& GridPathInfo, const int32 Rows, const
	                                             int32 Columns, const float GridWidth, const float GridHeight, const float CellWidth, const float CellHeight);
	
	UFUNCTION(BlueprintCallable, Category = "RandomPoints")
	static TArray<FVector2D> GeneratePoisonPoints(int32 Seed, int32 NoOfPoints, float MinimumDistance, int32 MaximumTries, const int32 Rows, const int32 Columns, const
	                                              float GridWidth, const float GridHeight, const float CellWidth, const float CellHeight, const TArray<bool>& GridPathInfo);

	UFUNCTION(BlueprintCallable, Category = "RandomPoints")
	static TArray<FVector2D> GeneratePoisonPointsAdvanced(int32 Seed, int32 MaximumPoints, float MinimumDistance, const int32 Rows, const int32 Columns, const
												  float GridWidth, const float GridHeight, const float CellWidth, const float CellHeight, const TArray<bool>& GridPathInfo);

	UFUNCTION(BlueprintCallable, Category = "RandomPoints")
	static FVector2D GetRandomPointAround2D(const FVector2D Point, const float MinimumDistance, const FRandomStream& RandomStream);

	UFUNCTION(BlueprintCallable, Category = "RandomPoints")
	static FVector GetRandomPointAround3D(const FVector Point, const float MinimumDistance, const FRandomStream& RandomStream);
	
};
