// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/GridDataGenerator.h"

#include "Generator/GeneratorHelpers.h"
#include "Kismet/KismetMathLibrary.h"

UGridDataGenerator::UGridDataGenerator()
{
	
}

#if WITH_EDITOR

void UGridDataGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	// Ensure FallOffStart is less than or equal to FallOffEnd
	if (FallOffStart > FallOffEnd)
	{
		FallOffStart = FallOffEnd;
	}

	// Ensure FallOffEnd is greater than or equal to FallOffStart
	if (FallOffEnd < FallOffStart)
	{
		FallOffEnd = FallOffStart;
	}
	
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	GenerateGridColorData();
}

#endif

void UGridDataGenerator::SortLevelRegions()
{
	LevelRegions.Sort();
	
	GenerateGridColorData();
}

void UGridDataGenerator::RefreshNoiseMap()
{
	NoiseMap = UGeneratorHelpers::GenerateNoiseMap1DWithFallOff(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset, FallOffStart, FallOffEnd, FallOffPower, FallOffInfluence, FallOffShape);
}

void UGridDataGenerator::InitGridColorData()
{
	const int32 TotalCells = Rows * Columns;
	const int32 RawColorsSize = TotalCells * 4;
	const int32 TotalRegions = LevelRegions.Num();
	
	if(NoiseColorsRaw.Num() != RawColorsSize)
	{
		NoiseColorsRaw.Empty();
		NoiseColorsRaw.Reserve(RawColorsSize);
		NoiseColorsRaw.AddUninitialized(RawColorsSize);
	}
		
	if(MapColorsRaw.Num() != RawColorsSize)
	{
		MapColorsRaw.Empty();
		MapColorsRaw.Reserve(RawColorsSize);
		MapColorsRaw.AddUninitialized(RawColorsSize);
	}
}

void UGridDataGenerator::GenerateGridColorData()
{
	InitGridColorData();
	RefreshNoiseMap();
	
	int32 Itr = 0;
	
	for(int Y=0; Y < Columns; ++Y)
	{
		for(int X=0; X < Rows; ++X)
		{
			const int32 CurrentIndex = UGeneratorHelpers::CoordinateToIndex(X, Y, Rows+1);
			const float CurrentHeight = NoiseMap[CurrentIndex];
			
			const FLinearColor CurrentNoiseColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, CurrentHeight);
			
			NoiseColorsRaw[Itr] = CurrentNoiseColor.B * 255;
			NoiseColorsRaw[Itr+1] = CurrentNoiseColor.G * 255;
			NoiseColorsRaw[Itr+2] = CurrentNoiseColor.R * 255;
			NoiseColorsRaw[Itr+3] = CurrentNoiseColor.A * 255;
			
			
			for(int32 RegionIndex= 0; RegionIndex < LevelRegions.Num(); RegionIndex++)
			{
				const FTerrainType& Region = LevelRegions[RegionIndex];
				
				if(CurrentHeight <= Region.MaxHeight)
				{
					MapColorsRaw[Itr] = Region.LinearColor.B * 255;
					MapColorsRaw[Itr+1] = Region.LinearColor.G * 255;
					MapColorsRaw[Itr+2] = Region.LinearColor.R * 255;
					MapColorsRaw[Itr+3] = Region.LinearColor.A * 255;
					
					break;
				}
			}

			Itr += 4;
		}
	}
	
}

float UGridDataGenerator::GetGridWidth() const
{
	return Rows * MeshScale.X;
}

float UGridDataGenerator::GetGridHeight() const
{
	return Columns * MeshScale.Y;
}

TArray<float> UGridDataGenerator::GetCurrentNoiseMap() const
{
	return NoiseMap;
}

TArray<float> UGridDataGenerator::GetCurrentNoiseMapNormalized() const
{
	TArray<float> NormalizedNoiseMap = NoiseMap;
	return UGeneratorHelpers::NormalizeNoiseMap1D(NormalizedNoiseMap, HeightMultiplierCurve, MeshHeightMultiplier, -0.5f, 1.0f);
}

void UGridDataGenerator::CalculateRegionData(TArray<uint8>& OutRegionIndexMapping, TArray<int32>& OutRegionCellCount)
{
	const int32 TotalCells = Rows * Columns;
	const int32 TotalRegions = LevelRegions.Num();
	
	OutRegionIndexMapping.Empty();
	OutRegionIndexMapping.Reserve(TotalCells);
	OutRegionIndexMapping.AddUninitialized(TotalCells);
	
	OutRegionCellCount.Empty();
	OutRegionCellCount.Reserve(TotalRegions);
	OutRegionCellCount.Init(0, TotalRegions);
		
	for(int X=0; X < Rows; ++X)
	{
		for(int Y=0; Y < Columns; ++Y)
		{
			const int32 CurrentCellIndex = UGeneratorHelpers::CoordinateToIndex(X, Y, Rows);
			const float CurrentCellNoise = GetCellNoiseAveraged(X, Y);
			
			for(int32 RegionIndex= 0; RegionIndex < LevelRegions.Num(); RegionIndex++)
			{
				const FTerrainType& Region = LevelRegions[RegionIndex];
				
				if(CurrentCellNoise <= Region.MaxHeight)
				{
					OutRegionIndexMapping[CurrentCellIndex] = RegionIndex;
					OutRegionCellCount[RegionIndex]++;
					
					break;
				}
			}
		}
	}
}

TArray<bool> UGridDataGenerator::GetGridPathInfo() const
{
	TArray<bool> PathInfo;
	PathInfo.Reserve(Rows * Columns);
	PathInfo.AddUninitialized(Rows * Columns);

	for(int X=0; X < Rows; ++X)
	{
		for(int Y=0; Y < Columns; ++Y)
		{
			const int32 CurrentCellIndex = UGeneratorHelpers::CoordinateToIndex(X, Y, Rows);
			const float CurrentCellNoise = GetCellNoiseAveraged(X, Y);
			
			for(int32 RegionIndex= 0; RegionIndex < LevelRegions.Num(); RegionIndex++)
			{
				const FTerrainType& Region = LevelRegions[RegionIndex];
				
				if(CurrentCellNoise <= Region.MaxHeight)
				{
					PathInfo[CurrentCellIndex] = Region.bCanSpawnObjects;
					break;
				}
			}
		}
	}

	return PathInfo;
}

float UGridDataGenerator::GetCellNoiseAveraged(const int32 InX, const int32 InY) const
{
	if((InX+1) * (InY+1) >= NoiseMap.Num())
	{
		return 0.0f;
	}
	
	const int32 BottomLeftIndex = UGeneratorHelpers::CoordinateToIndex(InX, InY, Rows+1);
	const int32 BottomRightIndex = UGeneratorHelpers::CoordinateToIndex(InX, InY+1, Rows+1);
	const int32 TopRightIndex = UGeneratorHelpers::CoordinateToIndex(InX+1, InY+1, Rows+1);
	const int32 TopLeftIndex = UGeneratorHelpers::CoordinateToIndex(InX+1, InY, Rows+1);

	return (NoiseMap[BottomLeftIndex] + NoiseMap[BottomRightIndex] + NoiseMap[TopRightIndex] + NoiseMap[TopLeftIndex])/4;
}
