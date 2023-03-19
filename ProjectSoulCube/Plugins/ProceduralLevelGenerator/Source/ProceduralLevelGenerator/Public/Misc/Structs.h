// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Structs.generated.h"

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
	
	float& operator[] (const int32& Index)
	{
		return FloatArray[Index];
	}

	const float& operator[] (const int32& Index) const
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

USTRUCT(BlueprintType)
struct FLinearColorArray
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FLinearColor> LinearColorArray;

public:	
	FLinearColorArray() = default;

	explicit FLinearColorArray(const TArray<FLinearColor>& InFloatArray) : LinearColorArray(InFloatArray) {}
	
	FLinearColor& operator[] (const int32& Index)
	{
		return LinearColorArray[Index];
	}

	const FLinearColor& operator[] (const int32& Index) const
	{
		return LinearColorArray[Index];
	}

	void Add(const FLinearColor& Value)
	{
		LinearColorArray.Add(Value);
	}

	int32 Num() const
	{
		return LinearColorArray.Num();
	}
};

USTRUCT(BlueprintType)
struct FTerrainType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString TerrainName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin=0.0f, ClampMax=1.0f))
	float MaxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LinearColor;
	
	FTerrainType() = default;
};

USTRUCT(BlueprintType)
struct FTerrainTypeArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTerrainType> Terrains;

	FTerrainType& operator[] (const int32& Index)
	{
		return Terrains[Index];
	}

	const FTerrainType& operator[] (const int32& Index) const
	{
		return Terrains[Index];
	}

	void Add(FTerrainType& Value)
	{
		float CurrentMinHeight = 0.0f;
		for(int Itr=0; Itr<Terrains.Num(); ++Itr)
		{
			if(Value.MaxHeight <= Terrains[Itr].MaxHeight)
			{
				Value.MinHeight = CurrentMinHeight;
				Terrains.Insert(Value, Itr);
			}
			else
			{
				CurrentMinHeight = Terrains[Itr].MaxHeight;
			}
		}
		Value.MinHeight = CurrentMinHeight;
		Terrains.Add(Value);
	}

	int32 Num() const
	{
		return Terrains.Num();
	}
};
