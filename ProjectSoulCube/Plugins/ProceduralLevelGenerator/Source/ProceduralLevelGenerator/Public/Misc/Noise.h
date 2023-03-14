// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Noise.generated.h"

USTRUCT(BlueprintType)
struct FFloatArray
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<float> FloatArray;

public:	
	FFloatArray() = default;

	explicit FFloatArray(const TArray<float>& InFloatArray) : FloatArray(InFloatArray) {}
	
	float operator[] (const int32& Index)
	{
		return FloatArray[Index];
	}

	float operator[] (const int32& Index) const
	{
		return FloatArray[Index];
	}

	void Add(const float& Value)
	{
		FloatArray.Add(Value);
	}

	int32 Num() const
	{
		return FloatArray.Num();
	}
};

/**
 * 
 */
UCLASS(meta=(ScriptName="Noise"))
class PROCEDURALLEVELGENERATOR_API UNoise : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Generate Noise Map", Keywords = "Perlin Noise Map Generate"), Category = "Noise")
	static TArray<FFloatArray> GenerateNoiseMap(const int32& MapX, const int& MapY, float Scale);
};
