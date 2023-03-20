﻿// Fill out your copyright notice in the Description page of Project Settings

#include "Misc/Noise.h"

#include "Kismet/KismetMathLibrary.h"
#include "Misc/Structs.h"

UNoise::UNoise(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

TArray<FFloatArray> UNoise::GenerateNoiseMap(const int& Seed, const int32& MapWidth, const int32& MapHeight, float Scale, const int32& Octaves, const float& Persistence, const float& Lacunarity, const
                                             FVector2D& Offset)
{
	// To avoid zero division error
	if(Scale <= 0){Scale = 0.0001f;}

	const float MapHalfWidth = MapWidth/2.0f;
	const float MapHalfHeight = MapHeight/2.0f;

	const FRandomStream RandomStream = FRandomStream(Seed);
	
	TArray<FVector2D> OctaveOffsets;
	for(int Itr=0; Itr < Octaves; ++Itr)
	{
		FVector2D CurrentOffset;
		CurrentOffset.X = UKismetMathLibrary::RandomFloatInRangeFromStream(-100000, 100000, RandomStream) + Offset.X;
		CurrentOffset.Y = UKismetMathLibrary::RandomFloatInRangeFromStream(-100000, 100000, RandomStream) + Offset.Y;
		OctaveOffsets.Add(CurrentOffset);
	}
	
	TArray<FFloatArray> NoiseMap;
	
	for(int Y=0; Y < MapHeight; ++Y)
	{
		FFloatArray CurrArray;
		for(int X=0; X < MapWidth; ++X)
		{
			float Amplitude = 1.0f;
			float Frequency = 1.0f;
			float NoiseHeight = 0.0f;
			
			for(int O=0; O < Octaves; ++O)
			{
				const FVector2D Sample = FVector2D((X - MapHalfWidth), (Y - MapHalfHeight)) / Scale * Frequency + OctaveOffsets[O];
				const float PerlinValue = FMath::PerlinNoise2D(Sample); // PerlinValue Range is between [-1,1].
				NoiseHeight += PerlinValue * Amplitude;
	
				Amplitude *= Persistence;
				Frequency *= Lacunarity;
			}
			
			NoiseHeight = NoiseHeight * 0.5f + 0.5f; // Normalize the perlin value to be in between [0,1].
			NoiseHeight = FMath::Clamp(NoiseHeight, 0.0f, 1.0f);
			CurrArray.Add(NoiseHeight);
		}
		NoiseMap.Add(CurrArray);
	}

	return NoiseMap;
}
