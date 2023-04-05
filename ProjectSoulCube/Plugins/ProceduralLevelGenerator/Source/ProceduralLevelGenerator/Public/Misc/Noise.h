// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Noise.generated.h"

struct FFloatArray;

/**
 * 
 */
UCLASS(meta=(ScriptName="Noise"))
class PROCEDURALLEVELGENERATOR_API UNoise : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Calculate Octave Offsets", Keywords = "Octave Offsets"), Category = "Noise")
	static TArray<FVector2D> CalculateOcataveOffsets(const int& Seed, const int32& Octaves, const FVector2D& Offset);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Calculate Perlin Value At Point", Keywords = "Perlin Noise Point"), Category = "Noise")
	static float CalculatePerlinValueAtPoint(const int32& MapHalfWidth, const int32& MapHalfHeight, const int32& PointX, const int32& PointY, float Scale,
											const int32& Octaves, const float& Persistence, const float& Lacunarity, const TArray<FVector2D>& OctaveOffsets);
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Generate Noise Map", Keywords = "Perlin Noise Map Generate"), Category = "Noise")
	static TArray<FFloatArray> GenerateNoiseMap(const int& Seed, const int32& MapWidth, const int32& MapHeight, float Scale, const int32& Octaves, const float& Persistence, const float& Lacunarity, const
	                                            FVector2D& Offset);

	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Normalize Noise Map", Keywords = "Perlin Noise Map Normalize"), Category = "Noise")
	static TArray<FFloatArray> NormalizeNoiseMap(const TArray<FFloatArray>& NoiseMap, const FRuntimeFloatCurve& NormalizeCurve, const float& MeshHeightMultiplier, const float& ClampMin = 0.0f, const float& ClampMax = 1.0f);

};
