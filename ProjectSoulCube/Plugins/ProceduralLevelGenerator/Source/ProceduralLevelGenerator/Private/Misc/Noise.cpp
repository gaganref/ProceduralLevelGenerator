// Fill out your copyright notice in the Description page of Project Settings

#include "Misc/Noise.h"

#include "Kismet/KismetMathLibrary.h"
#include "Misc/SimplexNoiseBPLibrary.h"
#include "Misc/Structs.h"

UNoise::UNoise(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

TArray<FFloatArray> UNoise::GenerateNoiseMap(const int32& MapX, const int32& MapY, float Scale, const int& Seed, const int32& Octaves, const float& Persistence, const float& Lacunarity)
{
	TArray<FFloatArray> NoiseMap;

	FRandomStream RandomStream = FRandomStream(Seed);
	
	TArray<FVector2D> OctaveOffsets;
	for(int Itr=0; Itr < Octaves; ++Itr)
	{
		FVector2D CurrentOffset;
		CurrentOffset.X = UKismetMathLibrary::RandomFloatInRangeFromStream(-100000, 100000, RandomStream);
		CurrentOffset.Y = UKismetMathLibrary::RandomFloatInRangeFromStream(-100000, 100000, RandomStream);
		OctaveOffsets.Add(CurrentOffset);
	}
	
	// Make Sure the scale is greater than zero to avoid zero division error.
	if(Scale <= 0)
	{
		Scale = 0.0001f;
	}
	
	for(int Y=0; Y < MapY; ++Y)
	{
		FFloatArray CurrArray;
		for(int X=0; X < MapX; ++X)
		{
			const FVector2D Sample = FVector2D(X, Y)/Scale;

			// getting the perlin value and converting it's range from [-1,1] to [0,1]
			// float PerlinValue = (USimplexNoiseBPLibrary::SimplexNoise2D(Sample.X, Sample.Y) * 0.5) + 0.5;
			float NoiseHeight = USimplexNoiseBPLibrary::GetSimplexNoise2D_EX(Sample.X, Sample.Y, Lacunarity, Persistence, Octaves, 1, true);

			// NoiseHeight = FMath::Clamp(NoiseHeight, MIN_flt, MAX_flt);
			// NoiseHeight = UKismetMathLibrary::NormalizeToRange(NoiseHeight, MIN_flt, MAX_flt);
			CurrArray.Add(NoiseHeight);
		}
		NoiseMap.Add(CurrArray);
	}
	
	return NoiseMap;
}
