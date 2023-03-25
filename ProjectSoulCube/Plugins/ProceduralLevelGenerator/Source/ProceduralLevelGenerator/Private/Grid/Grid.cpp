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
	
	// InitGrid();
	InitGrid_Test();

	
	float TimeElapsedInMs = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	float TimeElapsedInS = (FDateTime::UtcNow() - StartTime).GetTotalSeconds();
	UE_LOG(LogTemp, Error, TEXT("InitGrid - Excecution time = %f MilliSeconds = %f Seconds."), TimeElapsedInMs, TimeElapsedInS);
}

void UGrid::ClearData()
{
	NoiseColors.Empty();		// TODO: Can Improve Performance.
	MapColors.Empty();
}

void UGrid::InitGrid_Test()
{

	const int32 VertexCount = Rows * Columns * 4; // 4x vertices per quad/section
	const int32 TriangleCount = Rows * Columns * 2 * 3; // 2x3 vertex indexes per quad

	Vertices.Empty();
	Vertices.Reserve(VertexCount);
	Vertices.AddUninitialized(VertexCount);

	VertexColors.Empty();
	VertexColors.Reserve(VertexCount);
	VertexColors.AddUninitialized(VertexCount);

	Uvs.Empty();
	Uvs.Reserve(VertexCount);
	Uvs.AddUninitialized(VertexCount);

	Normals.Empty();
	Normals.Reserve(VertexCount);
	Normals.AddUninitialized(VertexCount);

	Tangents.Empty();
	Tangents.Reserve(VertexCount);
	Tangents.AddUninitialized(VertexCount);

	Triangles.Empty();
	Triangles.Reserve(VertexCount);
	Triangles.AddUninitialized(TriangleCount);
	
	
	const TArray<FFloatArray>& HeightMap = UNoise::GenerateNoiseMap(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset);

	NoiseColors.Reserve(Rows);
	MapColors.Reserve(Rows);

	const float TopLeftX = (Rows - 1) / -2.0f;
	const float TopLeftY = (Columns - 1) / -2.0f;
	
	int32 VertexIndex = 0;
	int32 TriangleIndex = 0;
	
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


			//Mesh

			const float CurrentMeshHeight = FMath::Clamp(HeightMultiplierCurve.GetRichCurveConst()->Eval(CurrentHeight), 0.0f, 1.0f) * MeshHeightMultiplier;
			
			// Setup a quad
			const int32 BottomLeftIndex = VertexIndex++;
			const int32 BottomRightIndex = VertexIndex++;
			const int32 TopRightIndex = VertexIndex++;
			const int32 TopLeftIndex = VertexIndex++;

			const float BottomLeftNoiseHeight = FMath::Clamp(HeightMultiplierCurve.GetRichCurveConst()->Eval(HeightMap[X][Y]), 0.0f, 1.0f) * MeshHeightMultiplier;
			const float BottomRightNoiseHeight = FMath::Clamp(HeightMultiplierCurve.GetRichCurveConst()->Eval(HeightMap[X][Y+1]), 0.0f, 1.0f) * MeshHeightMultiplier;
			const float TopRightIndexNoiseHeight = FMath::Clamp(HeightMultiplierCurve.GetRichCurveConst()->Eval(HeightMap[X+1][Y+1]), 0.0f, 1.0f) * MeshHeightMultiplier;
			const float TopLeftIndexNoiseHeight = FMath::Clamp(HeightMultiplierCurve.GetRichCurveConst()->Eval(HeightMap[X+1][Y]), 0.0f, 1.0f) * MeshHeightMultiplier;
			
			VertexColors[BottomLeftIndex] = CurrentRegionColor;
			VertexColors[BottomRightIndex] = CurrentRegionColor;
			VertexColors[TopRightIndex] = CurrentRegionColor;
			VertexColors[TopLeftIndex] = CurrentRegionColor;
			
			Vertices[BottomLeftIndex] = FVector(TopLeftX + X, TopLeftY + Y, BottomLeftNoiseHeight);
			Vertices[BottomRightIndex] = FVector(TopLeftX + X, TopLeftY + Y + 1, BottomRightNoiseHeight);
			Vertices[TopRightIndex] = FVector(TopLeftX + X + 1, TopLeftY + Y + 1, TopRightIndexNoiseHeight);
			Vertices[TopLeftIndex] = FVector(TopLeftX + X + 1, TopLeftY + Y, TopLeftIndexNoiseHeight);
			
			Uvs[BottomLeftIndex] = FVector2D(X/static_cast<float>(Rows), Y/static_cast<float>(Columns));
			Uvs[BottomRightIndex] = FVector2D(X/static_cast<float>(Rows), (Y+1)/static_cast<float>(Columns));
			Uvs[TopRightIndex] = FVector2D((X+1)/static_cast<float>(Rows), (Y+1)/static_cast<float>(Columns));
			Uvs[TopLeftIndex] = FVector2D((X+1)/static_cast<float>(Rows), Y/static_cast<float>(Columns));

			// Now create two triangles from those four vertices
			// The order of these (clockwise/counter-clockwise) dictates which way the normal will face. 
			Triangles[TriangleIndex++] = BottomLeftIndex;
			Triangles[TriangleIndex++] = TopRightIndex;
			Triangles[TriangleIndex++] = TopLeftIndex;

			Triangles[TriangleIndex++] = BottomLeftIndex;
			Triangles[TriangleIndex++] = BottomRightIndex;
			Triangles[TriangleIndex++] = TopRightIndex;

			// Normals
			const FVector NormalCurrent = FVector::CrossProduct(Vertices[BottomLeftIndex] - Vertices[TopLeftIndex], Vertices[TopLeftIndex] - Vertices[TopRightIndex]).GetSafeNormal();

			// If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
			Normals[BottomLeftIndex] = Normals[BottomRightIndex] = Normals[TopRightIndex] = Normals[TopLeftIndex] = NormalCurrent;

			// Tangents (perpendicular to the surface)
			const FVector SurfaceTangent = (FVector(TopLeftX + X, TopLeftY + Y, BottomLeftNoiseHeight) - FVector(TopLeftX + X, TopLeftY + Y + 1, BottomRightNoiseHeight)).GetSafeNormal();
			const FProcMeshTangent Tangent = FProcMeshTangent(SurfaceTangent, false);
			Tangents[BottomLeftIndex] = Tangents[BottomRightIndex] = Tangents[TopRightIndex] = Tangents[TopLeftIndex] = Tangent;

		}

		NoiseColors.Add(NoiseColorArray);
		MapColors.Add(MapColorArray);
	}
	
	NoiseColorsSize = NoiseColors.Num();
	MapColorsSize = MapColors.Num();

	MeshVertexSize = Vertices.Num();
	MeshUvsSize = Uvs.Num();
	MeshTriangleSize = Triangles.Num();
	
	MeshNormalsSize = Normals.Num();
	MeshTangentsSize = Tangents.Num();
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

	if(bUseFlatShading)
	{
		MeshData.FlatShading();
	}

	MeshData.RecalculateNormalsAndTangents();
	
	NoiseColorsSize = NoiseColors.Num();
	MapColorsSize = MapColors.Num();

	MeshVertexSize = MeshData.Vertices.Num();
	MeshUvsSize = MeshData.Uvs.Num();
	MeshTriangleSize = MeshData.Triangles.Num();
	
	MeshNormalsSize = MeshData.Normals.Num();
	MeshTangentsSize = MeshData.Tangents.Num();
}

