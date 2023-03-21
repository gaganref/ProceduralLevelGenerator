// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/GridDataGenerator.h"

#include "Kismet/KismetMathLibrary.h"
#include "Misc/Noise.h"


void UGridDataGenerator::GenerateData()
{
	ClearData();
	InitGrid();
}

void UGridDataGenerator::ClearData()
{
	NoiseColors.Empty();
	MapColors.Empty();
}

void UGridDataGenerator::InitGrid()
{
	UE_LOG(LogTemp, Warning, TEXT("Hi gagan, this is Grid Data Generator."));
	const TArray<FFloatArray>& HeightMap = UNoise::GenerateNoiseMap(Seed, Rows, Columns, Scale, Octaves, Persistence, Lacunarity, Offset);

	NoiseColors.Reserve(Rows);
	MapColors.Reserve(Rows);
	
	for(int X=0; X < Rows; ++X)
	{
		FLinearColorArray NoiseColorArray;
		NoiseColorArray.Reserve(Columns);

		FLinearColorArray MapColorArray;
		MapColorArray.Reserve(Columns);
		
		for(int Y=0; Y < Columns; ++Y)
		{
			const float CurrentHeight = HeightMap[X][Y];
			
			FLinearColor CurrentNoiseColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, CurrentHeight);
			NoiseColorArray.Add(CurrentNoiseColor);

			FLinearColor CurrentRegionColor = FLinearColor::Red;
			
			for(const auto& Region : LevelRegions)	// TODO: Can Improve Performance.
				{
				if(CurrentHeight <= Region.MaxHeight)
				{
					CurrentRegionColor = Region.LinearColor;
					break;
				}
				}
			MapColorArray.Add(CurrentRegionColor);
		}

		NoiseColors.Add(NoiseColorArray);
		MapColors.Add(MapColorArray);
	}
}
