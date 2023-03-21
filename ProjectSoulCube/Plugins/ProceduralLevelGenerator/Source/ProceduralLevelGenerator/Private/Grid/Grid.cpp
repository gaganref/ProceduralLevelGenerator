// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/Grid.h"

#include "Kismet/KismetMathLibrary.h"
#include "Misc/Noise.h"
#include "Misc/Structs.h"

void UGrid::PostInitProperties()
{
	Super::PostInitProperties();
}

void UGrid::PostReinitProperties()
{
	Super::PostReinitProperties();
}

#if WITH_EDITOR

void UGrid::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UGrid::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);

}

EDataValidationResult UGrid::IsDataValid(TArray<FText>& ValidationErrors)
{
	return Super::IsDataValid(ValidationErrors);
}

#endif

void UGrid::Initialize()
{
	ClearData();
	InitGrid();
}

void UGrid::ClearData()
{
	NoiseColors.Empty();
	MapColors.Empty();
}

void UGrid::InitGrid()
{
	UE_LOG(LogTemp, Warning, TEXT("Hi gagan from Grid"));
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
