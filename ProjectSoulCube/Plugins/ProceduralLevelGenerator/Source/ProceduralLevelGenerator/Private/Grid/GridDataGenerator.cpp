// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/GridDataGenerator.h"

#include "DisjointSet.h"
#include "Generator/GeneratorHelpers.h"
#include "Kismet/KismetMathLibrary.h"
#include "QuadTree/QuadTree.h"

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

	if(RegionIndexMapping.Num() != TotalCells)
	{
		RegionIndexMapping.Empty();
		RegionIndexMapping.Reserve(TotalCells);
		RegionIndexMapping.AddUninitialized(TotalCells);
	}
	
	if(RegionCellCount.Num() != TotalRegions)
	{
		RegionCellCount.Empty();
		RegionCellCount.Reserve(TotalRegions);
		RegionCellCount.AddUninitialized(TotalRegions);	
	}
	
	for(int32 Itr = 0; Itr < TotalRegions; Itr++)
	{
		RegionCellCount[Itr] = 0;
	}

	if(IsValidCell.Num() != TotalCells)
	{
		IsValidCell.Empty();
		IsValidCell.Reserve(TotalCells);
		IsValidCell.AddUninitialized(TotalCells);
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
					
					RegionCellCount[RegionIndex]++;
					
					const int32 CurrCellIndex = GetCellIndex(X, Y);
					
					RegionIndexMapping[CurrCellIndex] = RegionIndex;

					IsValidCell[CurrCellIndex] = Region.bCanSpawnObjects;
					// if(Region.ObjectSpawnProbability > 0)
					// {
					// 	IsValidCell[CurrCellIndex] = true;
					// }
					// else
					// {
					// 	IsValidCell[CurrCellIndex] = false;						
					// }
					
					break;
				}
			}

			Itr += 4;
		}
	}
	
}
//
// TArray<FVector2D> UGridDataGenerator::FindConnectedPoints2D(const FVector2D& InputPoint, const TArray<FVector2D>& TargetPoints)
// {
// 	FDisjointSet DisjointSet(Rows * Columns);
//
// 	FloodFill(DisjointSet);
//
// 	TArray<FVector2D> ConnectedPoints;
// 	
// 	const int32 InputPointIndex = GetCellIndex(InputPoint);
// 	const int32 InputPointRepresentative = DisjointSet.Find(InputPointIndex);
//
// 	for (const FVector2D& TargetPoint : TargetPoints) {
// 		const int32 TargetPointIndex = GetCellIndex(TargetPoint);
// 		const int32 TargetPointRepresentative = DisjointSet.Find(TargetPointIndex);
//
// 		if (InputPointRepresentative == TargetPointRepresentative) {
// 			ConnectedPoints.Add(TargetPoint);
// 		}
// 	}
//
// 	return ConnectedPoints;
// }
//
// TArray<FVector> UGridDataGenerator::FindConnectedPoints(const FVector& InputLocation, const TArray<FVector>& TargetLocations)
// {
// 	FDisjointSet DisjointSet(Rows * Columns);
//
// 	FloodFill(DisjointSet);
//
// 	TArray<FVector> ConnectedPoints;
// 	
// 	const int32 InputPointIndex = GetCellIndexByLocation(InputLocation);
// 	const int32 InputPointRepresentative = DisjointSet.Find(InputPointIndex);
//
// 	for (const FVector& TargetLocation : TargetLocations) {
// 		const int32 TargetPointIndex = GetCellIndexByLocation(TargetLocation);
// 		const int32 TargetPointRepresentative = DisjointSet.Find(TargetPointIndex);
//
// 		if (InputPointRepresentative == TargetPointRepresentative) {
// 			ConnectedPoints.Add(TargetLocation);
// 		}
// 	}
//
// 	return ConnectedPoints;
// }
//
// FVector2D UGridDataGenerator::GenerateRandomPointAround(const FVector2D& Point, const float& MinimumDistance, const FRandomStream& RandomStream) const
// {
// 	// start with non-uniform distribution
// 	const float R1 = RandomStream.FRand();
// 	const float R2 = RandomStream.FRand();
//
// 	// radius should be between MinDist and 2 * MinDist
// 	const float Radius = MinimumDistance * ( R1 + 1.0f );
//
// 	// random angle - // UE_PI * 2 * R2
// 	const float Angle = 6.283185307178f * R2;
//
// 	// the new point is generated around the point (x, y)
// 	FVector2D OutPoint;
// 	OutPoint.X = Point.X + Radius * cos( Angle );
// 	OutPoint.Y = Point.Y + Radius * sin( Angle );
//
// 	return OutPoint;
// }
//
FORCEINLINE int32 UGridDataGenerator::GetCellIndex(const int32& GridX, const int32& GridY) const
{
	return GridY * Rows + GridX; 
}

FORCEINLINE int32 UGridDataGenerator::GetCellIndex(const FVector2D& GridXY) const
{
	return GetCellIndex(GridXY.X, GridXY.Y); 
}

FVector2D UGridDataGenerator::GetCellIndex2D(const int32& GridIndex) const
{
	const int32 X = GridIndex / Rows;
	const int32 Y = GridIndex % Rows;
	return FVector2D(X, Y);
}

FORCEINLINE int32 UGridDataGenerator::GetCellIndexByLocation(const FVector& Location) const 
{
	int32 GridX = FMath::FloorToInt((Location.X + GetGridWidth() / 2.0f)/ MeshScale.X);
	int32 GridY = FMath::FloorToInt((Location.Y + GetGridHeight() / 2.0f) / MeshScale.Y);

	// Clamp values within grid dimensions
	GridX = FMath::Clamp(GridX, 0, Rows - 1);
	GridY = FMath::Clamp(GridY, 0, Columns - 1);

	return GetCellIndex(GridX, GridY);
}

FORCEINLINE int32 UGridDataGenerator::GetCellIndexByLocation(const FVector2D& Location) const 
{
	const FVector2D CellIndex2D = GetCellIndex2DByLocation(Location);

	return GetCellIndex(CellIndex2D.X, CellIndex2D.Y);
}

FORCEINLINE FVector2D UGridDataGenerator::GetCellIndex2DByLocation(const FVector2D& Location) const 
{
	int32 GridX = FMath::FloorToInt((Location.X + GetGridWidth() / 2.0f)/ MeshScale.X);
	int32 GridY = FMath::FloorToInt((Location.Y + GetGridHeight() / 2.0f) / MeshScale.Y);

	// Clamp values within grid dimensions
	GridX = FMath::Clamp(GridX, 0, Rows - 1);
	GridY = FMath::Clamp(GridY, 0, Columns - 1);

	return FVector2D(GridX, GridY);
}

FORCEINLINE bool UGridDataGenerator::IsPointInGrid(const FVector2D& Point) const
{
	const float HalfGridWidth = GetGridWidth()/2.0f;
	const float HalfGridHeight = GetGridHeight()/ 2.0f;
	return Point.X >= -HalfGridWidth && Point.X < HalfGridWidth && Point.Y >= -HalfGridHeight && Point.Y < HalfGridHeight;
}

FORCEINLINE bool UGridDataGenerator::IsPointInCell(const FVector2D& Point, const FVector2D& CellPosition) const
{
	return Point.X >= 0 && Point.X < GetGridWidth() && Point.Y >= 0 && Point.Y < GetGridHeight(); // TODO
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

int32 UGridDataGenerator::GetRegionCellCount(const int32 SectionIndex) const
{
	if(RegionCellCount.IsValidIndex(SectionIndex))
	{
		return RegionCellCount[SectionIndex];
	}

	return -1;
}

int32 UGridDataGenerator::GetRegionIndex(const int32 GridX, const int32 GridY) const
{
	const int32 CurrentCellIndex = GetCellIndex(GridX, GridY);

	if(RegionIndexMapping.IsValidIndex(CurrentCellIndex))
	{
		return RegionIndexMapping[CurrentCellIndex];
	}

	return -1;
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
