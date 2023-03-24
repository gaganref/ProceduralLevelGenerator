// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/Grid.h"

#include "Kismet/KismetMathLibrary.h"
#include "Misc/Noise.h"

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

void UGrid::GenerateData()
{
	Initialize();
}

void UGrid::Initialize()
{
	ClearData();
	FDateTime StartTime = FDateTime::UtcNow();
	InitGrid();
	float TimeElapsedInMs = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	float TimeElapsedInS = (FDateTime::UtcNow() - StartTime).GetTotalSeconds();
	UE_LOG(LogTemp, Error, TEXT("InitGrid - Excecution time = %f MilliSeconds = %f Seconds."), TimeElapsedInMs, TimeElapsedInS);
}

void UGrid::ClearData()
{
	NoiseColors.Empty();		// TODO: Can Improve Performance.
	MapColors.Empty();
}

void UGrid::InitGrid()
{
	const TArray<FFloatArray>& HeightMap = UNoise::GenerateNoiseMap(Seed, Rows, Columns, Scale, Octaves, Persistence, Lacunarity, Offset);

	NoiseColors.Reserve(Rows);
	MapColors.Reserve(Rows);

	const float TopLeftX = (Rows - 1) / -2.0f;
	const float TopLeftY = (Columns - 1) / -2.0f;

	MeshData = FCustomMeshData(Rows, Columns);
	
	int32 VertexIndex = 0;
	TArray<int32> VertexIndexArray;
	VertexIndexArray.Init(0, LevelRegions.Num());
	
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
			MeshData.AddVertexColor(CurrentRegionColor);

			const float CurrentMeshHeight = FMath::Clamp(HeightMultiplierCurve.GetRichCurveConst()->Eval(CurrentHeight), 0.0f, 1.0f) * MeshHeightMultiplier;
			MeshData.AddVertex(FVector(TopLeftX + X, TopLeftY + Y, CurrentMeshHeight));
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
}
