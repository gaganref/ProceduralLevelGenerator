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
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Generate Noise Map", Keywords = "Perlin Noise Map Generate"), Category = "Noise")
	static TArray<FFloatArray> GenerateNoiseMap(const int32& MapX, const int32& MapY, float Scale, const int& Seed,  const int32& Octaves = 4, const float& Persistence=0.6f, const float& Lacunarity=2.3f);
};
