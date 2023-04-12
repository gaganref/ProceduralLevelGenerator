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
UCLASS()
class PROCEDURALLEVELGENERATOR_API UGeneratorHelpers : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

protected:
	
	static TArray<FVector2D> CalculateOcataveOffsets(const int& Seed, const int32& Octaves, const FVector2D& Offset);

	void FloodFill(FDisjointSet& OutDisjointSet);

	FVector2D GenerateRandomPointAround(const FVector2D& Point, const float& MinimumDistance, const FRandomStream& RandomStream) const;

public:
	
	UFUNCTION(BlueprintCallable, Category = "Noise")
	static float CalculatePerlinValueAtPoint(const int32& MapHalfWidth, const int32& MapHalfHeight, const int32& PointX, const int32& PointY, float Scale,
											const int32& Octaves, const float& Persistence, const float& Lacunarity, const TArray<FVector2D>& OctaveOffsets);
	
	UFUNCTION(BlueprintCallable, Category = "Noise")
	static TArray<FFloatArray> GenerateNoiseMap(const int& Seed, const int32& MapWidth, const int32& MapHeight, float Scale, const int32& Octaves, const float& Persistence, const float& Lacunarity, const
												FVector2D& Offset);

	UFUNCTION(BlueprintCallable, Category = "Noise")
	static TArray<FFloatArray> GenerateNoiseMapNormalized(const int& Seed, const int32& MapWidth, const int32& MapHeight, float Scale, const int32& Octaves, const float& Persistence, const float& Lacunarity, const
												FVector2D& Offset, const UCurveFloat* NormalizeCurve, const float& MeshHeightMultiplier, const float& ClampMin, const float& ClampMax);

	UFUNCTION(BlueprintCallable, Category = "Noise")
	static TArray<FFloatArray> NormalizeNoiseMap(const TArray<FFloatArray>& NoiseMap, const UCurveFloat* NormalizeCurve, const float& MeshHeightMultiplier, const float& ClampMin = 0.0f, const float& ClampMax = 1.0f);

	UFUNCTION(BlueprintCallable, Category = "RandomPoints")
	static TArray<FVector2D> GeneratePoisonPoints(int32 NoOfPoints, float MinimumDistance, int32 NoOfTries);

	UFUNCTION(BlueprintCallable, Category = "RandomPoints")
	static TArray<FVector2D> GeneratePoisonPointsAdvanced(int32 NoOfPoints, float MinimumDistance, int32 NoOfTries);
};
