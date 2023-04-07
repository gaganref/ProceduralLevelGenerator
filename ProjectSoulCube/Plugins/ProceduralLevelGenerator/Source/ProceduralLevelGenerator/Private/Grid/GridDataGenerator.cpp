// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/GridDataGenerator.h"

#include "Kismet/KismetMathLibrary.h"
#include "Misc/Noise.h"

#if WITH_EDITOR

void UGridDataGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);
	
	GenerateGridColorData();
}

#endif

void UGridDataGenerator::SortLevelRegions()
{
	LevelRegions.Sort();
	
	GenerateGridColorData();
}

void UGridDataGenerator::InitGridColorData()
{
	const int32 RawColorsSize = Rows * Columns * 4;
	const int32 TotalCells = Rows * Columns;
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
	
	if(Cells.Num() != TotalCells)
	{
		Cells.Empty();
		Cells.Reserve(TotalCells);
		Cells.AddUninitialized(TotalCells);
	}

	if(Cells.Num() != TotalRegions)
	{
		SectionCellCount.Empty();
		SectionCellCount.Reserve(TotalRegions);
		SectionCellCount.AddUninitialized(TotalRegions);	
	}
	
	for(int32 Itr = 0; Itr < TotalRegions; Itr++)
	{
		SectionCellCount[Itr] = 0;
	}
}

void UGridDataGenerator::GenerateGridColorData()
{
	InitGridColorData();

	NoiseData = UNoise::GenerateNoiseMap(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset);
	
	int32 Itr = 0;
	
	for(int Y=0; Y < Columns; ++Y)
	{
		for(int X=0; X < Rows; ++X)
		{
			const float CurrentHeight = NoiseData[X][Y];
			
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
					
					const int32 CurrCellIndex = Rows * Y + X;
					Cells[CurrCellIndex] = FCell(X, Y, RegionIndex);
					
					SectionCellCount[RegionIndex]++;
					
					break;
				}
			}

			Itr += 4;
		}
	}
	
}

TArray<FLevelSection> UGridDataGenerator::GenerateMeshSectionData(const FVector& ComponentLocation /**= FVector(0.0f, 0.0f, 0.0f)*/)
{
	GenerateGridColorData();	// To make Sure that the data is  up to date
	
	TArray<FLevelSection> OutMeshSectionsData;
	OutMeshSectionsData.Reserve(LevelRegions.Num());

	const TArray<FFloatArray>&  NoiseDataNormalized = UNoise::NormalizeNoiseMap(NoiseData, HeightMultiplierCurve, MeshHeightMultiplier, -0.5f, 1.0f);

	for(int32 Itr = 0; Itr < LevelRegions.Num(); Itr++)
	{
		OutMeshSectionsData.Add(FLevelSection(SectionCellCount[Itr]));
	}

	// This is the Grid based BottomLeft XY values that are unscaled as we will be scaling them in CreateQuad
	const float BottomLeftX = (Rows - 1) / -2.0f;
	const float BottomLeftY = (Columns - 1) / -2.0f;
	
	for(int X=0; X < Rows; ++X)
	{
		for(int Y=0; Y < Columns; ++Y)
		{
			const int32 CurrentCellIndex = Rows * Y + X;
			const int32 CurrentRegionIndex = Cells[CurrentCellIndex].RegionIndex;

			OutMeshSectionsData[CurrentRegionIndex].CreateQuad(NoiseDataNormalized, X, Y, BottomLeftX, BottomLeftY, Rows, Columns, MeshScale);
		}
	}

	return OutMeshSectionsData;
}

TArray<FVector> UGridDataGenerator::GenerateValidObjectLocations()
{
	TArray<FVector> OutArray;
	
	GenerateGridColorData();	// To make Sure that the data is  up to date

	const TArray<FFloatArray>& NoiseDataNormalized = UNoise::NormalizeNoiseMap(NoiseData, HeightMultiplierCurve, MeshHeightMultiplier);

	const float BottomLeftX = (Rows - 1) / -2.0f;
	const float BottomLeftY = (Columns - 1) / -2.0f;
	
	// for(int X=0; X < Rows; ++X)
	// {
	// 	for(int Y=0; Y < Columns; ++Y)
	// 	{
	// 		
	// 	}
	// }

	return OutArray;
}
