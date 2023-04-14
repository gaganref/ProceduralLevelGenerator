// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator/GeneratorHelpers.h"

#include "DisjointSet.h"
#include "Kismet/KismetMathLibrary.h"
#include "Misc/Structs.h"

int32 UGeneratorHelpers::CoordinateToIndex(const int32 X, const int32 Y, const int32 Rows)
{
	return Y * Rows + X; 
}

int32 UGeneratorHelpers::CoordinateToIndexWithFVector2D(const FVector2D Coordinate, const int32 Rows)
{
	return CoordinateToIndex(Coordinate.X, Coordinate.Y, Rows);
}

FVector2D UGeneratorHelpers::IndexToCoordinate(const int32 Index, const int32 Rows)
{
	const int32 X = Index / Rows;
	const int32 Y = Index % Rows;
	return FVector2D(X, Y);
}

int32 UGeneratorHelpers::LocationToIndex(const FVector2D Location, const float GridWidth, const float GridHeight,
	const float CellWidth, const float CellHeight, const int32 Rows)
{
	const int32 X = FMath::FloorToInt((Location.X + GridWidth / 2.0f)/ CellWidth);
	const int32 Y = FMath::FloorToInt((Location.Y + GridHeight / 2.0f) / CellHeight);

	return CoordinateToIndex(X, Y, Rows);
}

FVector2D UGeneratorHelpers::LocationToCoordinate(const FVector2D Location, const float GridWidth,
                                                  const float GridHeight, const float CellWidth, const float CellHeight)
{
	const int32 X = FMath::FloorToInt((Location.X + GridWidth / 2.0f)/ CellWidth);
	const int32 Y = FMath::FloorToInt((Location.Y + GridHeight / 2.0f) / CellHeight);

	return FVector2D(X, Y);
}

bool UGeneratorHelpers::IsPointInGrid(const FVector2D Point, const float Width, const float Height)
{
	const float HalfGridWidth = Width/2.0f;
	const float HalfGridHeight = Height/ 2.0f;
	return Point.X >= -HalfGridWidth && Point.X < HalfGridWidth && Point.Y >= -HalfGridHeight && Point.Y < HalfGridHeight;
}

TArray<FFloatArray> UGeneratorHelpers::GenerateFallOffMap(const int32 MapWidth, const int32 MapHeight, const float FallOffStart, const float FallOffEnd, const float FallOffPower, const float FallOffInfluence, TEnumAsByte<EFallOffShape> FallOffShape)
{
	FFloatArray FillArray;
	FillArray.Reserve(MapHeight);
	FillArray.AddUninitialized(MapHeight);

	TArray<FFloatArray> FallOffMap;
	FallOffMap.Reserve(MapWidth);
	FallOffMap.Init(FillArray, MapWidth);

	const FVector2D Center = FVector2D(MapWidth / 2.0f, MapHeight / 2.0f);
	
	// for(int Y=0; Y < MapHeight; ++Y)
	// {
	// 	for(int X=0; X < MapWidth; ++X)
	// 	{
	// 		// Circular Method
	// 		const float DistanceFromCenter = FVector2D::Distance(Center, FVector2D(X, Y));
	// 		const float MaxDistance = Center.GetMax();
	// 		float FalloffValue = FMath::GetRangePct(FallOffStart * MaxDistance, FallOffEnd * MaxDistance, DistanceFromCenter);
	// 		FalloffValue = FMath::Clamp(FalloffValue, 0.0f, 1.0f);
	// 		FallOffMap[X][Y] = FMath::Pow(FalloffValue, FallOffPower) * FallOffInfluence;
	//
	// 		
	// 		// // Square Method Extra Control
	// 		// const float DistanceX = FMath::Abs(Center.X - X) / Center.X;
	// 		// const float DistanceY = FMath::Abs(Center.Y - Y) / Center.Y;
	// 		// const float DistanceFromCenter = FMath::Max(DistanceX, DistanceY);
	// 		//
	// 		// float FalloffValue = FMath::GetRangePct(FallOffStart, FallOffEnd, DistanceFromCenter);
	// 		// FalloffValue = FMath::Clamp(FalloffValue, 0.0f, 1.0f);
	// 		// FalloffValue = FMath::Pow(FalloffValue, FallOffPower);
	// 		//
	// 		// FallOffMap[X][Y] = DistanceFromCenter * FalloffValue * FallOffInfluence;
	// 	}
	// }

	for(int Y=0; Y < MapHeight; ++Y)
	{
		for(int X=0; X < MapWidth; ++X)
		{
			FVector2D CurrentPosition = FVector2D(X, Y);
			float DistanceFromCenter;

			switch (FallOffShape)
			{
				case Fos_Circle:
				{
					DistanceFromCenter = FVector2D::Distance(Center, CurrentPosition);
					break;
				}
				
				case Fos_Square:
				{
					DistanceFromCenter = FMath::Max(FMath::Abs(Center.X - CurrentPosition.X), FMath::Abs(Center.Y - CurrentPosition.Y));
					break;
				}
				
				case Fos_Diamond:
				{
					DistanceFromCenter = FMath::Abs(Center.X - CurrentPosition.X) + FMath::Abs(Center.Y - CurrentPosition.Y);
					break;
				}
				
				default:
				{
					DistanceFromCenter = FVector2D::Distance(Center, CurrentPosition);
					break;
				}
			}
			
			const float MaxDistance = Center.GetMax();
			float FalloffValue = FMath::GetRangePct(FallOffStart * MaxDistance, FallOffEnd * MaxDistance, DistanceFromCenter);
			FalloffValue = FMath::Clamp(FalloffValue, 0.0f, 1.0f);
			FallOffMap[X][Y] = FMath::Pow(FalloffValue, FallOffPower) * FallOffInfluence;
		}
	}

	return FallOffMap;
}

TArray<FVector2D> UGeneratorHelpers::CalculateOcataveOffsets(const int32 Seed, const int32 Octaves, const FVector2D Offset)
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

void UGeneratorHelpers::FloodFill(FDisjointSet& OutDisjointSet, const TArray<bool>& GridPathInfo, const int32 Rows, const int32 Columns)
{
	if(GridPathInfo.IsEmpty() || GridPathInfo.Num() != Rows * Columns-1)
	{
		return;
	}
	
	constexpr int32 DirectionX[] = {-1, 0, 1, 0};
	constexpr int32 DirectionY[] = {0, 1, 0, -1};

	for (int32 Y = 0; Y < Columns; ++Y)
	{
		for (int32 X = 0; X < Rows; ++X)
		{
			
			const int32 CurrentIndex = CoordinateToIndex(X, Y, Rows);

			if (!GridPathInfo[CurrentIndex]) {
				continue;
			}

			for (int32 Direction = 0; Direction < 4; ++Direction) {
				const int32 NewX = X + DirectionX[Direction];
				const int32 NewY = Y + DirectionY[Direction];

				if (NewX < 0 || NewX >= Rows || NewY < 0 || NewY >= Columns || !GridPathInfo[CurrentIndex]) {
					continue;
				}

				const int32 NeighborIndex = CoordinateToIndex(X, Y, Rows);
				OutDisjointSet.Union(CurrentIndex, NeighborIndex);
			}
		}
	}
}

float UGeneratorHelpers::CalculatePerlinValueAtPoint(const int32 MapHalfWidth, const int32 MapHalfHeight,
                                                     const int32 PointX, const int32 PointY, float Scale, const int32 Octaves, const float Persistence,
                                                     const float Lacunarity, const TArray<FVector2D>& OctaveOffsets)
{
	// To avoid zero division error
	if(Scale <= 0){Scale = 0.0001f;}
	
	float NoiseHeight = 0.0f;
	float Amplitude = 1.0f;
	float Frequency = 1.0f;
			
	for(int O=0; O < Octaves; ++O)
	{
		const FVector2D Sample = FVector2D((PointX - MapHalfWidth), (PointY - MapHalfHeight)) / Scale * Frequency + OctaveOffsets[O] * Frequency;
		const float PerlinValue = FMath::PerlinNoise2D(Sample); // PerlinValue Range is between [-1,1].
		NoiseHeight += PerlinValue * Amplitude;
		Amplitude *= Persistence;
		Frequency *= Lacunarity;
	}
			
	NoiseHeight = NoiseHeight * 0.5f + 0.5f; // Normalize the perlin value to be in between [0,1].
	return FMath::Clamp(NoiseHeight, 0.0f, 1.0f);
}

TArray<FFloatArray> UGeneratorHelpers::GenerateNoiseMap(const int32 Seed, const int32 MapWidth, const int32 MapHeight,
                                                        float Scale, const int32 Octaves, const float Persistence, const float Lacunarity, const FVector2D Offset)
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

TArray<FFloatArray> UGeneratorHelpers::GenerateNoiseMapNormalized(const int Seed, const int32 MapWidth,
                                                                  const int32 MapHeight, float Scale, const int32 Octaves, const float Persistence, const float Lacunarity,
                                                                  const FVector2D Offset, const UCurveFloat* NormalizeCurve, const float MeshHeightMultiplier,
                                                                  const float ClampMin, const float ClampMax)
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

TArray<FFloatArray> UGeneratorHelpers::NormalizeNoiseMap(const TArray<FFloatArray>& NoiseMap,
                                                         const UCurveFloat* NormalizeCurve, const float MeshHeightMultiplier, const float ClampMin, const float ClampMax)
{
	if(NoiseMap.IsEmpty())
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

FVector2D UGeneratorHelpers::GenerateInitialPoissonPoint(const FRandomStream& RandomStream, const int32 MaximumTries, const TArray<bool>& GridPathInfo, const int32 Rows,
	const int32 Columns, const float GridWidth, const float GridHeight, const float CellWidth, const float CellHeight)
{
	if(GridPathInfo.IsEmpty() || GridPathInfo.Num() != Rows * Columns-1)
	{
		return FVector2D(0.0f, 0.0f);
	}

	const float HalfGridWidth = GridWidth/2.0f;
	const float HalfGridHeight = GridHeight/ 2.0f;

	FVector2D InitialPoint;
	
	for(int Tries=0; Tries<MaximumTries; Tries++)
	{
		InitialPoint.X = RandomStream.FRandRange(-HalfGridWidth, HalfGridWidth);
		InitialPoint.Y = RandomStream.FRandRange(-HalfGridHeight, HalfGridHeight);

		const int32 PointIndexInGrid = LocationToIndex(InitialPoint, GridWidth, GridHeight, CellWidth, CellHeight, Rows);
		
		if(IsPointInGrid(InitialPoint, GridWidth, GridHeight) && GridPathInfo[PointIndexInGrid])
		{
			break;
		}
	}

	return InitialPoint;
}

TArray<FVector2D> UGeneratorHelpers::GeneratePoisonPoints(const int32 Seed, const int32 NoOfPoints, const float MinimumDistance, const int32 MaximumTries, const int32 Rows, const
                                                          int32 Columns, const float GridWidth, const float GridHeight, const float CellWidth, const float CellHeight, const TArray<bool>& GridPathInfo)
{

	if(GridPathInfo.IsEmpty() || GridPathInfo.Num() != Rows * Columns-1)
	{
		TArray<FVector2D> Empty;
		return Empty;
	}
	
	const FRandomStream RandomStream(Seed);

	TArray<int32> Grid;
	Grid.Empty();
	Grid.Reserve(Rows * Columns);
	Grid.Init(-1, Rows * Columns);

	TArray<FVector2D> Points;
	TArray<FVector2D> ProcessList;

	if(!NoOfPoints)
	{
		return Points;
	}
	
	const FVector2D InitialPoint = GenerateInitialPoissonPoint(RandomStream, MaximumTries, GridPathInfo, Rows, Columns, GridWidth, GridHeight, CellWidth, CellHeight);
	
	ProcessList.Push(InitialPoint);
	Points.Push(InitialPoint);

	while(!ProcessList.IsEmpty() && Points.Num() < NoOfPoints)
	{
		const FVector2D CurrentPoint = ProcessList.Pop();

		for (int32 i = 0; i < MaximumTries; i++)
		{
			const FVector2D NewPoint = GetRandomPointAround2D(CurrentPoint, MinimumDistance, RandomStream);

			if(IsPointInGrid(NewPoint, GridWidth, GridHeight))
			{
				const FVector2D GridPoint = LocationToCoordinate(NewPoint, GridWidth, GridHeight, CellWidth, CellHeight);

				if (GridPathInfo[CoordinateToIndex(GridPoint.X, GridPoint.Y, Rows)])
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
								const int32 CurrentCellIndex = CoordinateToIndex(Neighbour.X, Neighbour.Y, Rows);
								if(Grid[CurrentCellIndex] != -1)
								{
									FVector2D NeighborPoint = Points[Grid[CurrentCellIndex]];
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
						Grid[CoordinateToIndex(GridPoint.X, GridPoint.Y, Rows)] = NewIndex;

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

TArray<FVector2D> UGeneratorHelpers::GeneratePoisonPointsAdvanced(const int32 Seed, const int32 MaximumPoints, const float MinimumDistance, const int32 Rows, const
														  int32 Columns, const float GridWidth, const float GridHeight, const float CellWidth, const float CellHeight, const TArray<bool>& GridPathInfo)
{
	const FRandomStream RandomStream(Seed);
	
	TArray<FVector2D> Points;
	
	const float HalfGridWidth = GridWidth/2.0f;
	const float HalfGridHeight = GridHeight/ 2.0f;
	
	TArray<int32> Grid;
	Grid.Empty();
	Grid.Reserve(Rows * Columns);
	Grid.Init(-1, Rows * Columns);
	
    // const FVector2D InitialPoint(RandomStream.FRandRange(-HalfGridWidth, HalfGridWidth), RandomStream.FRandRange(-HalfGridHeight, HalfGridHeight));

	const FVector2D InitialPoint = GenerateInitialPoissonPoint(RandomStream, 5, GridPathInfo, Rows, Columns, GridWidth, GridHeight, CellWidth, CellHeight);
	
	Points.Add(InitialPoint);
    TArray<FVector2D> ActiveList = {InitialPoint};

    while (ActiveList.Num() > 0 && Points.Num() < MaximumPoints)
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
        	
        	const FVector2D GridPoint = LocationToCoordinate(NewPoint, GridWidth, GridHeight, CellWidth, CellHeight);
        	
        	if(GridPathInfo[CoordinateToIndex(GridPoint.X, GridPoint.Y, Rows)])
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
        					const int32 NeighbourIndex = Grid[CoordinateToIndex(Neighbour.X, Neighbour.Y, Rows)];
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
        			Grid[CoordinateToIndex(GridPoint.X, GridPoint.Y, Rows)] = Points.Num() - 1;

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


FVector2D UGeneratorHelpers::GetRandomPointAround2D(const FVector2D Point, const float MinimumDistance, const FRandomStream& RandomStream)
{
	// start with non-uniform distribution
	const float R1 = RandomStream.FRand();
	const float R2 = RandomStream.FRand();

	// radius should be between MinDist and 2 * MinDist
	const float Radius = MinimumDistance * ( R1 + 1.0f );

	// random angle - // UE_PI * 2 * R2
	const float Angle = 6.283185307178f * R2;

	// the new point is generated around the point (x, y)
	const float X = Point.X + Radius * cosf(Angle);
	const float Y = Point.Y + Radius * sinf(Angle);
	return FVector2D(X, Y);
}

FVector UGeneratorHelpers::GetRandomPointAround3D(const FVector Point, const float MinimumDistance, const FRandomStream& RandomStream)
{
	// start with non-uniform distribution
	const float R1 = RandomStream.FRand();
	const float R2 = RandomStream.FRand();
	const float R3 = RandomStream.FRand();

	// radius should be between MinDist and 2 * MinDist
	const float Radius = MinimumDistance * (R1 + 1.0f);

	// random angles - azimuth (0 to 2 * PI) and inclination (0 to PI)
	const float Azimuth = 6.283185307178f * R2;
	const float Inclination = 3.141592653589f * R3;

	// the new point is generated around the point (x, y, z)
	const float X = Point.X + Radius * sinf(Inclination) * cosf(Azimuth);
	const float Y = Point.Y + Radius * sinf(Inclination) * sinf(Azimuth);
	const float Z = Point.Z + Radius * cosf(Inclination);

	return FVector(X, Y, Z);
}
