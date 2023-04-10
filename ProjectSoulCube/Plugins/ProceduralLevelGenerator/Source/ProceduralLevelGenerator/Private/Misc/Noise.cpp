// Fill out your copyright notice in the Description page of Project Settings

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
	OctaveOffsets.AddUninitialized(Octaves);
	
	const FRandomStream RandomStream = FRandomStream(Seed);
	
	for(int Itr=0; Itr < Octaves; ++Itr)
	{
		OctaveOffsets[Itr].X = UKismetMathLibrary::RandomFloatInRangeFromStream(-100000, 100000, RandomStream) + Offset.X;
		OctaveOffsets[Itr].Y = UKismetMathLibrary::RandomFloatInRangeFromStream(-100000, 100000, RandomStream) + Offset.Y;
	}

	return OctaveOffsets;
}

float UNoise::CalculatePerlinValueAtPoint(const int32& MapHalfWidth, const int32& MapHalfHeight, const int32& PointX,
	const int32& PointY, float Scale, const int32& Octaves, const float& Persistence, const float& Lacunarity,
	const TArray<FVector2D>& OctaveOffsets)
{
	// To avoid zero division error
	if(Scale <= 0){Scale = 0.0001f;}
	
	float NoiseHeight = 0.0f;
	float Amplitude = 1.0f;
	float Frequency = 1.0f;
			
	for(int O=0; O < Octaves; ++O)
	{
		const FVector2D Sample = FVector2D((PointX - MapHalfWidth), (PointY - MapHalfHeight)) / Scale * Frequency + OctaveOffsets[O] * Frequency;
		// const FVector2D Sample = FVector2D((PointX - MapHalfWidth), (PointY - MapHalfHeight)) / Scale * Frequency + OctaveOffsets[O];
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
	
	
	FFloatArray FillArray;
	FillArray.Reserve(MapHeight);
	FillArray.AddUninitialized(MapHeight);

	TArray<FFloatArray> NoiseMap;
	NoiseMap.Reserve(MapWidth);
	NoiseMap.Init(FillArray, MapWidth);
	
	for(int X=0; X < MapWidth; ++X)	
	{
		for(int Y=0; Y < MapHeight; ++Y)
		{
			NoiseMap[X][Y] = CalculatePerlinValueAtPoint(MapHalfWidth, MapHalfHeight, X, Y, Scale, Octaves, Persistence, Lacunarity, OctaveOffsets);
		}
	}

	return NoiseMap;
}

TArray<FFloatArray> UNoise::GenerateNoiseMapNormalized(const int& Seed, const int32& MapWidth, const int32& MapHeight, float Scale, const int32& Octaves, const float& Persistence, const float& Lacunarity, const
											 FVector2D& Offset, const UCurveFloat* NormalizeCurve, const float& MeshHeightMultiplier, const float& ClampMin, const float& ClampMax)
{
	// To avoid zero division error
	if(Scale <= 0){Scale = 0.0001f;}

	const float MapHalfWidth = MapWidth/2.0f;
	const float MapHalfHeight = MapHeight/2.0f;
	
	const TArray<FVector2D>& OctaveOffsets = CalculateOcataveOffsets(Seed, Octaves, Offset);
	
	
	FFloatArray FillArray;
	FillArray.Reserve(MapHeight);
	FillArray.AddUninitialized(MapHeight);

	TArray<FFloatArray> NoiseMap;
	NoiseMap.Reserve(MapWidth);
	NoiseMap.Init(FillArray, MapWidth);
	
	for(int X=0; X < MapWidth; ++X)	
	{
		for(int Y=0; Y < MapHeight; ++Y)
		{
			const float PerlinValue = CalculatePerlinValueAtPoint(MapHalfWidth, MapHalfHeight, X, Y, Scale, Octaves, Persistence, Lacunarity, OctaveOffsets); 
			NoiseMap[X][Y] = FMath::Clamp(NormalizeCurve->GetFloatValue(PerlinValue), ClampMin, ClampMax) * MeshHeightMultiplier;
		}
	}

	return NoiseMap;
}

TArray<FFloatArray> UNoise::NormalizeNoiseMap(const TArray<FFloatArray>& NoiseMap, const UCurveFloat* NormalizeCurve, const float& MeshHeightMultiplier, const float& ClampMin, const float& ClampMax)
{
	if(NoiseMap.Num() < 1)
	{
		return NoiseMap;
	}

	const int32 Width = NoiseMap.Num();
	const int32 Height = NoiseMap[0].Num();
	
	TArray<FFloatArray> OutNoiseMap = NoiseMap;

	for (int Y = 0; Y < Height; ++Y)
	{
		for (int X = 0; X < Width; ++X)
		{
			OutNoiseMap[X][Y] = FMath::Clamp(NormalizeCurve->GetFloatValue(OutNoiseMap[X][Y]), ClampMin, ClampMax) * MeshHeightMultiplier;
		}
	}

	return OutNoiseMap;
}
