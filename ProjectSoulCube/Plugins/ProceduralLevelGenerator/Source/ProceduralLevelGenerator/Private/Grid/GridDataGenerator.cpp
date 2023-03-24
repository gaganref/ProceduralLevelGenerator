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
	NoiseColors.Empty();		// TODO: Can Improve Performance.
	MapColors.Empty();
}

void UGridDataGenerator::InitGrid()
{
	UE_LOG(LogTemp, Warning, TEXT("Hi gagan, this is Grid Data Generator."));
	const TArray<FFloatArray>& HeightMap = UNoise::GenerateNoiseMap(Seed, Rows, Columns, Scale, Octaves, Persistence, Lacunarity, Offset);

	NoiseColors.Reserve(Rows);
	MapColors.Reserve(Rows);

	const float TopLeftX = (Rows - 1) / -2.0f;
	const float TopLeftY = (Columns - 1) / -2.0f;

	MeshData = FCustomMeshData(Rows, Columns);
	int32 VertexIndex = 0;
	
	for(int X=0; X < Rows; ++X)
	{
		FLinearColorArray NoiseColorArray;
		NoiseColorArray.Reserve(Columns);
		
		FLinearColorArray MapColorArray;
		MapColorArray.Reserve(Columns);
		
		for(int Y=0; Y < Columns; ++Y)
		{
			const float CurrentHeight = HeightMap[X][Y];

			// Adding colors for noise plane generation
			FLinearColor CurrentNoiseColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, CurrentHeight);
			NoiseColorArray.Add(CurrentNoiseColor);

			// Adding colors for map plane generation
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

			// Adding mesh data
			MeshData.AddVertex(FVector(TopLeftX + X, TopLeftY + Y, HeightMap[X][Y] * MeshHeightMultiplier));
			MeshData.AddUv(FVector2D(X/static_cast<float>(Rows), Y/static_cast<float>(Columns)));

			if((X < Rows-1) && (Y < Columns-1))
			{
				MeshData.AddTriangle(VertexIndex, VertexIndex + Rows + 1, VertexIndex + Rows);
				MeshData.AddTriangle(VertexIndex + Rows + 1, VertexIndex, VertexIndex + 1);
			}

			++VertexIndex;
		}

		NoiseColors.Add(NoiseColorArray);
		MapColors.Add(MapColorArray);
	}

	NoiseColorsSize = NoiseColors.Num();
	MapColorsSize = MapColors.Num();

	MeshVertexSize = MeshData.Vertices.Num();
	MeshUvsSize = MeshData.Uvs.Num();
	MeshTriangleSize = MeshData.Triangles.Num();
	
	MeshData.RecalculateNormalsAndTangents();

	MeshNormalsSize = MeshData.Normals.Num();
	MeshTangentsSize = MeshData.Tangents.Num();

	Vertices = MeshData.Vertices;
}
