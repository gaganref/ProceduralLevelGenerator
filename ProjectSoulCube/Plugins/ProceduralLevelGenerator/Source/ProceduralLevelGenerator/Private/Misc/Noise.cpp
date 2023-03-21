﻿// Fill out your copyright notice in the Description page of Project Settings

#include "Misc/Noise.h"

#include "Kismet/KismetMathLibrary.h"
#include "Misc/Structs.h"

UNoise::UNoise(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	
}

TArray<FVector2D> UNoise::CalculateOcataveOffsets(const int& Seed, const int32& Octaves, const FVector2D& Offset)
{
	TArray<FVector2D> OctaveOffsets;
	OctaveOffsets.Reserve(Octaves);
	const FRandomStream RandomStream = FRandomStream(Seed);
	
	for(int Itr=0; Itr < Octaves; ++Itr)
	{
		FVector2D CurrentOffset;
		CurrentOffset.X = UKismetMathLibrary::RandomFloatInRangeFromStream(-100000, 100000, RandomStream) + Offset.X;
		CurrentOffset.Y = UKismetMathLibrary::RandomFloatInRangeFromStream(-100000, 100000, RandomStream) + Offset.Y;
		OctaveOffsets.Add(CurrentOffset);
	}

	return OctaveOffsets;
}

float UNoise::CalculatePerlinValueAtPoint(const int32& MapHalfWidth, const int32& MapHalfHeight, const int32& PointX,
	const int32& PointY, float Scale, const int32& Octaves, const float& Persistence, const float& Lacunarity,
	const TArray<FVector2D>& OctaveOffsets)
{
	float NoiseHeight = 0.0f;
	float Amplitude = 1.0f;
	float Frequency = 1.0f;
			
	for(int O=0; O < Octaves; ++O)
	{
		const FVector2D Sample = FVector2D((PointX - MapHalfWidth), (PointY - MapHalfHeight)) / Scale * Frequency + OctaveOffsets[O];
		const float PerlinValue = FMath::PerlinNoise2D(Sample); // PerlinValue Range is between [-1,1].
		NoiseHeight += PerlinValue * Amplitude;
		Amplitude *= Persistence;
		Frequency *= Lacunarity;
	}
			
	NoiseHeight = NoiseHeight * 0.5f + 0.5f; // Normalize the perlin value to be in between [0,1].
	return FMath::Clamp(NoiseHeight, 0.0f, 1.0f);
}

TArray<FFloatArray> UNoise::GenerateNoiseMap(const int& Seed, const int32& MapWidth, const int32& MapHeight, float Scale, const int32& Octaves, const float& Persistence, const float& Lacunarity, const
                                             FVector2D& Offset)
{
	// To avoid zero division error
	if(Scale <= 0){Scale = 0.0001f;}

	const float MapHalfWidth = MapWidth/2.0f;
	const float MapHalfHeight = MapHeight/2.0f;
	
	const TArray<FVector2D>& OctaveOffsets = CalculateOcataveOffsets(Seed, Octaves, Offset);
	
	TArray<FFloatArray> NoiseMap;
	NoiseMap.Reserve(MapWidth);
	
	for(int X=0; X < MapWidth; ++X)	
	{
		FFloatArray CurrArray;
		CurrArray.Reserve(MapHeight);
		
		for(int Y=0; Y < MapHeight; ++Y)
		{
			const float NoiseHeight = CalculatePerlinValueAtPoint(MapHalfWidth, MapHalfHeight, X, Y, Scale, Octaves, Persistence, Lacunarity, OctaveOffsets);
			CurrArray.Add(NoiseHeight);
		}
		NoiseMap.Add(CurrArray);
	}

	return NoiseMap;
}
