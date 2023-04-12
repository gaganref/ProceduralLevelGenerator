// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/GridDataGenerator.h"

#include "DisjointSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/Noise.h"
#include "QuadTree/QuadTree.h"

UGridDataGenerator::UGridDataGenerator()
{
	
}

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

	if(RegionIndexMapping.Num() != TotalCells)
	{
		RegionIndexMapping.Empty();
		RegionIndexMapping.Reserve(TotalCells);
		RegionIndexMapping.AddUninitialized(TotalCells);
	}
	
	if(SectionCellCount.Num() != TotalRegions)
	{
		SectionCellCount.Empty();
		SectionCellCount.Reserve(TotalRegions);
		SectionCellCount.AddUninitialized(TotalRegions);	
	}
	
	for(int32 Itr = 0; Itr < TotalRegions; Itr++)
	{
		SectionCellCount[Itr] = 0;
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

	// NoiseData = UNoise::GenerateNoiseMap(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset);
	const TArray<FFloatArray>& NoiseData = UNoise::GenerateNoiseMap(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset);
	
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
					
					SectionCellCount[RegionIndex]++;

					const int32 CurrCellIndex = GetCellIndex(X, Y);
					
					RegionIndexMapping[CurrCellIndex] = RegionIndex;

					if(Region.ObjectSpawnProbability > 0)
					{
						IsValidCell[CurrCellIndex] = true;
					}
					else
					{
						IsValidCell[CurrCellIndex] = false;						
					}
					
					break;
				}
			}

			Itr += 4;
		}
	}
	
}

TArray<FLevelSection> UGridDataGenerator::GenerateMeshSectionData()
{
	GenerateGridColorData();	// To make Sure that the data is  up to date
	
	TArray<FLevelSection> OutMeshSectionsData;
	OutMeshSectionsData.Reserve(LevelRegions.Num());

	const TArray<FFloatArray>&  NoiseDataNormalized = UNoise::GenerateNoiseMapNormalized(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset, HeightMultiplierCurve, MeshHeightMultiplier, -0.5f, 1.0f);

	
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
			const int32 CurrentRegionIndex = RegionIndexMapping[GetCellIndex(X, Y)];
			
			OutMeshSectionsData[CurrentRegionIndex].CreateQuad(NoiseDataNormalized, X, Y, BottomLeftX, BottomLeftY, Rows, Columns, MeshScale);
		}
	}

	return OutMeshSectionsData;
}

TArray<FVector2D> UGridDataGenerator::GeneratePoisonDiskPoints(int32 NoOfPoints, float MinimumDistance, int32 NoOfTries)
{
	GenerateGridColorData();	// To make Sure that the data is  up to date
	
	const FRandomStream RandomStream(Seed);

	const float HalfGridWidth = GetGridWidth()/2.0f;
	const float HalfGridHeight = GetGridHeight()/ 2.0f;
	
	TArray<int32> GridInfo;
	GridInfo.Empty();
	GridInfo.Reserve(Rows * Columns);
	GridInfo.Init(-1, Rows * Columns);

	if (MinimumDistance < 0.0f)
	{
		MinimumDistance = 1.0f;
	}

	TArray<FVector2D> Points;
	TArray<FVector2D> ProcessList;

	if(!NoOfPoints)
	{
		return Points;
	}
	
	FVector2D InitialPoint;

	while(!IsPointInGrid(InitialPoint))
	{
		InitialPoint.X = RandomStream.FRandRange(-HalfGridWidth, HalfGridWidth);
		InitialPoint.Y = RandomStream.FRandRange(-HalfGridHeight, HalfGridHeight);

		GridInfo[GetCellIndexByLocation(InitialPoint)] = 0;
	}

	ProcessList.Push(InitialPoint);
	Points.Push(InitialPoint);

	while(!ProcessList.IsEmpty() && Points.Num() < NoOfPoints)
	{
		const FVector2D CurrentPoint = ProcessList.Pop();

		for (int32 i = 0; i < NoOfTries; i++)
		{
			const FVector2D NewPoint = GenerateRandomPointAround(CurrentPoint, MinimumDistance, RandomStream);

			if(IsPointInGrid(NewPoint))
			{
				const FVector2D GridPoint = GetCellIndex2DByLocation(NewPoint);

				if (IsValidCell[GetCellIndex(GridPoint)])
				{
					bool IsValid = true;
					for (int32 YOffset = -2; YOffset <= 2 && IsValid; YOffset++)
					{
						for (int32 XOffset = -2; XOffset <= 2 && IsValid; XOffset++)
						{
							FVector2D Neighbour = GridPoint;
							Neighbour.X += XOffset;
							Neighbour.Y += YOffset;

							if(Neighbour.X >= 0 && Neighbour.X < Rows && Neighbour.Y >= 0 && Neighbour.Y < Columns)
							{
								const int32 CurrentCellIndex = GetCellIndex(Neighbour);
								if(GridInfo[CurrentCellIndex] != -1)
								{
									FVector2D NeighborPoint = Points[GridInfo[CurrentCellIndex]];
									if (FVector2D::DistSquared(NewPoint, NeighborPoint) < MinimumDistance * MinimumDistance)
									{
										IsValid = false;
									}
								}
							}
						}
					}

					if (IsValid)
					{
						const int32 NewIndex = Points.Add(NewPoint);
						ProcessList.Push(NewPoint);
						GridInfo[GetCellIndex(GridPoint)] = NewIndex;

						if(Points.Num() >= NoOfPoints)
						{
							break;
						}
					}
				}
			}
		}
	}

	return Points;
}

TArray<FVector2D> UGridDataGenerator::GeneratePoisonDiskPointsEvenly(float MinimumDistance, int32 MaxPoints)
{
	const FRandomStream RandomStream(Seed);
	
	TArray<FVector2D> Points;

    const FVector2D CellSize(MinimumDistance / FMath::Sqrt(2.0), MinimumDistance / FMath::Sqrt(2.0));
	
	const float HalfGridWidth = GetGridWidth()/2.0f;
	const float HalfGridHeight = GetGridHeight()/ 2.0f;
	
	TArray<int32> GridInfo;
	GridInfo.Empty();
	GridInfo.Reserve(Rows * Columns);
	GridInfo.Init(-1, Rows * Columns);
	
    const  FVector2D InitialPoint(RandomStream.FRandRange(-HalfGridWidth, HalfGridWidth), RandomStream.FRandRange(-HalfGridHeight, HalfGridHeight));
    Points.Add(InitialPoint);
    TArray<FVector2D> ActiveList = {InitialPoint};

    while (ActiveList.Num() > 0 && Points.Num() < MaxPoints)
    {
	    const int32 ActiveIndex = RandomStream.RandRange(0, ActiveList.Num() - 1);
        FVector2D ActivePoint = ActiveList[ActiveIndex];

        bool Found = false;
        for (int32 k = 0; k < 30; ++k)
        {
	        const float R = RandomStream.FRandRange(MinimumDistance, 2 * MinimumDistance);
	        const float Theta = RandomStream.FRandRange(0, 2 * PI);

        	FVector2D NewPoint = ActivePoint + FVector2D(FMath::Cos(Theta), FMath::Sin(Theta)) * R;
        	if (NewPoint.X < -HalfGridWidth || NewPoint.X >= HalfGridWidth || NewPoint.Y < -HalfGridHeight || NewPoint.Y >= HalfGridHeight)
        	{
        		continue;
        	}
        	
        	const FVector2D GridPoint = GetCellIndex2DByLocation(NewPoint);

        	if (IsValidCell[GetCellIndex(GridPoint)])
        	{
        		bool IsValid = true;
        		for (int32 YOffset = -2; YOffset <= 2 && IsValid; ++YOffset)
        		{
        			for (int32 XOffset = -2; XOffset <= 2 && IsValid; ++XOffset)
        			{
        				FVector2D Neighbour = GridPoint;
        				Neighbour.X += XOffset;
        				Neighbour.Y += YOffset;

        				if(Neighbour.X >= 0 && Neighbour.X < Rows && Neighbour.Y >= 0 && Neighbour.Y < Columns)
        				{
        					const int32 NeighbourIndex = GridInfo[GetCellIndex(Neighbour)];
        					if(NeighbourIndex!= -1)
        					{
        						FVector2D NeighborPoint = Points[NeighbourIndex];
        						if (FVector2D::DistSquared(NewPoint, NeighborPoint) < MinimumDistance * MinimumDistance)
        						{
        							IsValid = false;
        						}
        					}
        				}
        			}
        		}

        		if (IsValid)
        		{
        			Points.Add(NewPoint);
        			ActiveList.Add(NewPoint);
        			GridInfo[GetCellIndex(GridPoint)] = Points.Num() - 1;

        			Found = true;
        			break;
        		}	
        	}
        }

        if (!Found)
        {
            ActiveList.RemoveAt(ActiveIndex);
        }
    }

    return Points;
}

void UGridDataGenerator::FloodFill(FDisjointSet& OutDisjointSet)
{
	constexpr int32 DirectionX[] = {-1, 0, 1, 0};
	constexpr int32 DirectionY[] = {0, 1, 0, -1};

	for (int32 y = 0; y < Columns; ++y) {
		for (int32 x = 0; x < Rows; ++x) {
			
			const int32 CurrentIndex = GetCellIndex(x, y);
			
			if (!IsValidCell[CurrentIndex]) {
				continue;
			}

			for (int32 Direction = 0; Direction < 4; ++Direction) {
				int32 NewX = x + DirectionX[Direction];
				int32 NewY = y + DirectionY[Direction];

				if (NewX < 0 || NewX >= Rows || NewY < 0 || NewY >= Columns || !IsValidCell[CurrentIndex]) {
					continue;
				}

				const int32 NeighborIndex = GetCellIndex(NewX, NewY);
				OutDisjointSet.Union(CurrentIndex, NeighborIndex);
			}
		}
	}
}

TArray<FVector2D> UGridDataGenerator::FindConnectedPoints2D(const FVector2D& InputPoint, const TArray<FVector2D>& TargetPoints)
{
	FDisjointSet DisjointSet(Rows * Columns);

	FloodFill(DisjointSet);

	TArray<FVector2D> ConnectedPoints;
	
	const int32 InputPointIndex = GetCellIndex(InputPoint);
	const int32 InputPointRepresentative = DisjointSet.Find(InputPointIndex);

	for (const FVector2D& TargetPoint : TargetPoints) {
		const int32 TargetPointIndex = GetCellIndex(TargetPoint);
		const int32 TargetPointRepresentative = DisjointSet.Find(TargetPointIndex);

		if (InputPointRepresentative == TargetPointRepresentative) {
			ConnectedPoints.Add(TargetPoint);
		}
	}

	return ConnectedPoints;
}

TArray<FVector> UGridDataGenerator::FindConnectedPoints(const FVector& InputLocation, const TArray<FVector>& TargetLocations)
{
	FDisjointSet DisjointSet(Rows * Columns);

	FloodFill(DisjointSet);

	TArray<FVector> ConnectedPoints;
	
	const int32 InputPointIndex = GetCellIndexByLocation(InputLocation);
	const int32 InputPointRepresentative = DisjointSet.Find(InputPointIndex);

	for (const FVector& TargetLocation : TargetLocations) {
		const int32 TargetPointIndex = GetCellIndexByLocation(TargetLocation);
		const int32 TargetPointRepresentative = DisjointSet.Find(TargetPointIndex);

		if (InputPointRepresentative == TargetPointRepresentative) {
			ConnectedPoints.Add(TargetLocation);
		}
	}

	return ConnectedPoints;
}

FVector2D UGridDataGenerator::GenerateRandomPointAround(const FVector2D& Point, const float& MinimumDistance, const FRandomStream& RandomStream) const
{
	// start with non-uniform distribution
	const float R1 = RandomStream.FRand();
	const float R2 = RandomStream.FRand();

	// radius should be between MinDist and 2 * MinDist
	const float Radius = MinimumDistance * ( R1 + 1.0f );

	// random angle - // UE_PI * 2 * R2
	const float Angle = 6.283185307178f * R2;

	// the new point is generated around the point (x, y)
	FVector2D OutPoint;
	OutPoint.X = Point.X + Radius * cos( Angle );
	OutPoint.Y = Point.Y + Radius * sin( Angle );

	return OutPoint;
}

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
	// return Point.X >= 0 && Point.X < GetGridWidth() && Point.Y >= 0 && Point.Y < GetGridHeight();	// this only checks for positive side
	
	const float HalfGridWidth = GetGridWidth()/2.0f;
	const float HalfGridHeight = GetGridHeight()/ 2.0f;
	return Point.X >= -HalfGridWidth && Point.X < HalfGridWidth && Point.Y >= -HalfGridHeight && Point.Y < HalfGridHeight;
}

FORCEINLINE bool UGridDataGenerator::IsPointInCell(const FVector2D& Point, const FVector2D& CellPosition) const
{
	return Point.X >= 0 && Point.X < GetGridWidth() && Point.Y >= 0 && Point.Y < GetGridHeight(); // TODO
}

FORCEINLINE float UGridDataGenerator::GetGridWidth() const
{
	return Rows * MeshScale.X;
}

FORCEINLINE float UGridDataGenerator::GetGridHeight() const
{
	return Columns * MeshScale.Y;
}

