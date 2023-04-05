// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/GridDataGenerator.h"

#include "Kismet/KismetMathLibrary.h"
#include "Misc/Noise.h"

#if WITH_EDITOR

void UGridDataGenerator::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	UObject::PostEditChangeProperty(PropertyChangedEvent);

	GenerateGridData(false);
}

#endif


void UGridDataGenerator::GenerateData()
{
	GenerateGridData(true);
}

void UGridDataGenerator::GenerateGridData(const bool bCanGenerateMeshData)
{
	NoiseData = UNoise::GenerateNoiseMap(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset);
	NoiseDataNormalized = UNoise::NormalizeNoiseMap(NoiseData, HeightMultiplierCurve, MeshHeightMultiplier);
	
	GenerateGridColorData();
	DisplayFinalGridColorData();

	if(bCanGenerateMeshData)
	{
		GenerateGridMeshData();
		DisplayFinalGridMeshData();
	}
}

void UGridDataGenerator::ClearGridColorData()
{
	NoiseColorsRaw.Empty();
	NoiseColors.Empty();
	MapColorsRaw.Empty();
	MapColors.Empty();
}

void UGridDataGenerator::ClearGridMeshData()
{
	Vertices.Empty();
	VertexColors.Empty();
	Uvs.Empty();
	Normals.Empty();
	Tangents.Empty();
	Triangles.Empty();
}

void UGridDataGenerator::InitGridColorData()
{
	FLinearColorArray EmptyColorArray;
	EmptyColorArray.Reserve(Columns);
	EmptyColorArray.AddUninitialized(Columns);

	NoiseColorsRaw.Reserve(Rows * Columns * 4);
	NoiseColorsRaw.AddUninitialized(Rows * Columns * 4);

	NoiseColors.Reserve(Rows);
	NoiseColors.Init(EmptyColorArray, Rows);

	MapColorsRaw.Reserve(Rows * Columns * 4);
	MapColorsRaw.AddUninitialized(Rows * Columns * 4);

	MapColors.Reserve(Rows);
	MapColors.Init(EmptyColorArray, Rows);
}

void UGridDataGenerator::InitGridMeshData()
{
	const int32 TriangleCount = Rows * Columns * 2 * 3; // 2x3 vertex indexes per quad and each tri has 3 vertices and it doesn't share them
	
	Vertices.Reserve(TriangleCount);
	Vertices.AddUninitialized(TriangleCount);
	
	VertexColors.Reserve(TriangleCount);
	VertexColors.AddUninitialized(TriangleCount);

	Uvs.Reserve(TriangleCount);
	Uvs.AddUninitialized(TriangleCount);

	Normals.Reserve(TriangleCount);
	Normals.AddUninitialized(TriangleCount);

	Tangents.Reserve(TriangleCount);
	Tangents.AddUninitialized(TriangleCount);

	Triangles.Reserve(TriangleCount);
	Triangles.AddUninitialized(TriangleCount);
}

void UGridDataGenerator::GenerateGridColorData()
{
	ClearGridColorData();
	InitGridColorData();
	
	int32 Itr = 0;
	
	for(int Y=0; Y < Columns; ++Y)
	{
		for(int X=0; X < Rows; ++X)
		{
			const float CurrentHeight = NoiseData[X][Y];
			
			const FLinearColor CurrentNoiseColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, CurrentHeight);

			NoiseColors[X][Y] = CurrentNoiseColor;
			
			NoiseColorsRaw[Itr] = CurrentNoiseColor.B * 255;
			NoiseColorsRaw[Itr+1] = CurrentNoiseColor.G * 255;
			NoiseColorsRaw[Itr+2] = CurrentNoiseColor.R * 255;
			NoiseColorsRaw[Itr+3] = CurrentNoiseColor.A * 255;

			int32 CurrentRegionIndex = 0;
			for(const auto& Region : LevelRegions)	// TODO: Can Improve Performance.
			{
				if(CurrentHeight <= Region.MaxHeight)
				{
					MapColors[X][Y] = Region.LinearColor;

					MapColorsRaw[Itr] = Region.LinearColor.B * 255;
					MapColorsRaw[Itr+1] = Region.LinearColor.G * 255;
					MapColorsRaw[Itr+2] = Region.LinearColor.R * 255;
					MapColorsRaw[Itr+3] = Region.LinearColor.A * 255;
					break;
				}

				CurrentRegionIndex++;
			}

			Itr += 4;
		}
	}
}

void UGridDataGenerator::GenerateGridMeshData()
{
	ClearGridMeshData();
	InitGridMeshData();
	
	const float TopLeftX = (Rows - 1) / -2.0f;
	const float TopLeftY = (Columns - 1) / -2.0f;
	
	int32 VertexIndex = 0;
	int32 TriangleIndex = 0;
	
	for(int X=0; X < Rows; ++X)
	{
		for(int Y=0; Y < Columns; ++Y)
		{
			FLinearColor CurrentRegionColor = MapColors[X][Y];

			//Mesh

			// Noise Heights for quad vertex points
			
			const float BottomLeftNoiseHeight = NoiseDataNormalized[X][Y];
			const float BottomRightNoiseHeight = NoiseDataNormalized[X][Y+1];
			const float TopRightIndexNoiseHeight = NoiseDataNormalized[X+1][Y+1];
			const float TopLeftIndexNoiseHeight = NoiseDataNormalized[X+1][Y];
			
			// Triangle A
			const int32 BottomLeftIndex_A = VertexIndex++;
			const int32 TopRightIndex_A = VertexIndex++;
			const int32 TopLeftIndex_A = VertexIndex++;

			VertexColors[BottomLeftIndex_A] = CurrentRegionColor;
			VertexColors[TopRightIndex_A] = CurrentRegionColor;
			VertexColors[TopLeftIndex_A] = CurrentRegionColor;

			Vertices[BottomLeftIndex_A] = FVector(TopLeftX + X, TopLeftY + Y, BottomLeftNoiseHeight);
			Vertices[TopRightIndex_A] = FVector(TopLeftX + X + 1, TopLeftY + Y + 1, TopRightIndexNoiseHeight);
			Vertices[TopLeftIndex_A] = FVector(TopLeftX + X + 1, TopLeftY + Y, TopLeftIndexNoiseHeight);

			Uvs[BottomLeftIndex_A] = FVector2D(X/static_cast<float>(Rows), Y/static_cast<float>(Columns));
			Uvs[TopRightIndex_A] = FVector2D((X+1)/static_cast<float>(Rows), (Y+1)/static_cast<float>(Columns));
			Uvs[TopLeftIndex_A] = FVector2D((X+1)/static_cast<float>(Rows), Y/static_cast<float>(Columns));

			// The order of these (clockwise/counter-clockwise) dictates which way the normal will face.
			Triangles[TriangleIndex++] = BottomLeftIndex_A;
			Triangles[TriangleIndex++] = TopRightIndex_A;
			Triangles[TriangleIndex++] = TopLeftIndex_A;

			// Calculate triangle edge vectors and normal
			const FVector Edge21 = Vertices[TopRightIndex_A] - Vertices[TopLeftIndex_A];
			const FVector Edge20 = Vertices[BottomLeftIndex_A] - Vertices[TopLeftIndex_A];
			const FVector Normal_A = (Edge21 ^ Edge20).GetSafeNormal();
			
			// If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
			Normals[BottomLeftIndex_A] = Normals[TopRightIndex_A] = Normals[TopLeftIndex_A] = Normal_A;

			// Tangents (perpendicular to the surface)
			const FVector SurfaceTangent_A = (Vertices[BottomLeftIndex_A] + Vertices[TopRightIndex_A] + Vertices[TopLeftIndex_A] / 3).GetSafeNormal();
			const FProcMeshTangent Tangent_A = FProcMeshTangent(SurfaceTangent_A, false);
			Tangents[BottomLeftIndex_A] = Tangents[TopRightIndex_A] = Tangents[TopLeftIndex_A] = Tangent_A;
			
			
			// Triangle B
			const int32 BottomLeftIndex_B = VertexIndex++;
			const int32 BottomRightIndex_B = VertexIndex++;
			const int32 TopRightIndex_B = VertexIndex++;

			VertexColors[BottomLeftIndex_B] = CurrentRegionColor;
			VertexColors[BottomRightIndex_B] = CurrentRegionColor;
			VertexColors[TopRightIndex_B] = CurrentRegionColor;

			Vertices[BottomLeftIndex_B] = FVector(TopLeftX + X, TopLeftY + Y, BottomLeftNoiseHeight);
			Vertices[BottomRightIndex_B] = FVector(TopLeftX + X, TopLeftY + Y + 1, BottomRightNoiseHeight);
			Vertices[TopRightIndex_B] = FVector(TopLeftX + X + 1, TopLeftY + Y + 1, TopRightIndexNoiseHeight);

			Uvs[BottomLeftIndex_B] = FVector2D(X/static_cast<float>(Rows), Y/static_cast<float>(Columns));
			Uvs[BottomRightIndex_B] = FVector2D(X/static_cast<float>(Rows), (Y+1)/static_cast<float>(Columns));
			Uvs[TopRightIndex_B] = FVector2D((X+1)/static_cast<float>(Rows), (Y+1)/static_cast<float>(Columns));

			// The order of these (clockwise/counter-clockwise) dictates which way the normal will face.
			Triangles[TriangleIndex++] = BottomLeftIndex_B;
			Triangles[TriangleIndex++] = BottomRightIndex_B;
			Triangles[TriangleIndex++] = TopRightIndex_B;

			// Calculate triangle edge vectors and normal
			const FVector Edge43 = Vertices[BottomLeftIndex_B] - Vertices[BottomRightIndex_B];
			const FVector Edge45 = Vertices[TopRightIndex_B] - Vertices[BottomRightIndex_B];
			const FVector NormalCurrent_B = (Edge43 ^ Edge45).GetSafeNormal();
			
			// If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
			Normals[BottomLeftIndex_B] = Normals[BottomRightIndex_B] = Normals[TopRightIndex_B] = NormalCurrent_B;
			
			// Setup a quad
			
			// Tangents (perpendicular to the surface)
			const FVector SurfaceTangent_B = (Vertices[BottomLeftIndex_B] + Vertices[BottomRightIndex_B] + Vertices[TopRightIndex_B] / 3).GetSafeNormal();
			const FProcMeshTangent Tangent_B = FProcMeshTangent(SurfaceTangent_B, false);
			Tangents[BottomLeftIndex_B] = Tangents[BottomRightIndex_B] = Tangents[TopRightIndex_B] = Tangent_B;

		}
	}
}

void UGridDataGenerator::DisplayFinalGridColorData()
{
	NoiseColorsRawSize = NoiseColorsRaw.Num();
	MapColorsRawSize = MapColorsRaw.Num();

	if(NoiseColors.Num() > 0)
	{
		NoiseColorsSize = FVector2D(NoiseColors.Num(), NoiseColors[0].Num());
	}
	if(MapColors.Num() > 0)
	{
		MapColorsSize = FVector2D(MapColors.Num(), MapColors[0].Num());
	}
}

void UGridDataGenerator::DisplayFinalGridMeshData()
{
	MeshVertexSize = Vertices.Num();
	MeshUvsSize = Uvs.Num();
	MeshTriangleSize = Triangles.Num();
	MeshNormalsSize = Normals.Num();
	MeshTangentsSize = Tangents.Num();
}

void UGridDataGenerator::InitColorsOnly()
{
	const TArray<FFloatArray>& HeightMap = UNoise::GenerateNoiseMap(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset);

	FLinearColorArray EmptyColorArray;
	EmptyColorArray.Reserve(Columns);
	EmptyColorArray.AddUninitialized(Columns);

	NoiseColorsRaw.Empty();
	NoiseColorsRaw.Reserve(Rows * Columns * 4);
	NoiseColorsRaw.AddUninitialized(Rows * Columns * 4);
	
	NoiseColors.Empty();
	NoiseColors.Reserve(Rows);
	NoiseColors.Init(EmptyColorArray, Rows);

	MapColorsRaw.Empty();
	MapColorsRaw.Reserve(Rows * Columns * 4);
	MapColorsRaw.AddUninitialized(Rows * Columns * 4);
	
	MapColors.Empty();
	MapColors.Reserve(Rows);
	MapColors.Init(EmptyColorArray, Rows);

	int32 Itr = 0;
	
	
	for(int Y=0; Y < Columns; ++Y)
	{
		FLinearColorArray NoiseColorArray;
		NoiseColorArray.Reserve(Columns);
		
		FLinearColorArray MapColorArray;
		MapColorArray.Reserve(Columns);
		
		for(int X=0; X < Rows; ++X)
		{
			const float CurrentHeight = HeightMap[X][Y];
			
			FLinearColor CurrentNoiseColor = UKismetMathLibrary::LinearColorLerp(FLinearColor::Black, FLinearColor::White, CurrentHeight);

			NoiseColors[X][Y] = CurrentNoiseColor;
			
			NoiseColorsRaw[Itr] = CurrentNoiseColor.B * 255;
			NoiseColorsRaw[Itr+1] = CurrentNoiseColor.G * 255;
			NoiseColorsRaw[Itr+2] = CurrentNoiseColor.R * 255;
			NoiseColorsRaw[Itr+3] = CurrentNoiseColor.A * 255;
			
			FLinearColor CurrentRegionColor = FLinearColor::Red;
			
			for(const auto& Region : LevelRegions)	// TODO: Can Improve Performance.
			{
				if(CurrentHeight <= Region.MaxHeight)
				{
					CurrentRegionColor = Region.LinearColor;
					break;
				}
			}
			
			MapColors[X][Y] = CurrentRegionColor;

			MapColorsRaw[Itr] = CurrentRegionColor.B * 255;
			MapColorsRaw[Itr+1] = CurrentRegionColor.G * 255;
			MapColorsRaw[Itr+2] = CurrentRegionColor.R * 255;
			MapColorsRaw[Itr+3] = CurrentRegionColor.A * 255;

			Itr += 4;
		}
	}
	
	// NoiseColorsSize = NoiseColors.Num();
	// MapColorsSize = MapColors.Num();
	NoiseColorsRawSize = NoiseColorsRaw.Num();
}

void UGridDataGenerator::InitGrid()
{
	const int32 TriangleCount = Rows * Columns * 2 * 3; // 2x3 vertex indexes per quad and each tri has 3 vertices and it doesn't share them

	Vertices.Empty();
	Vertices.Reserve(TriangleCount);
	Vertices.AddUninitialized(TriangleCount);

	VertexColors.Empty();
	VertexColors.Reserve(TriangleCount);
	VertexColors.AddUninitialized(TriangleCount);

	Uvs.Empty();
	Uvs.Reserve(TriangleCount);
	Uvs.AddUninitialized(TriangleCount);

	Normals.Empty();
	Normals.Reserve(TriangleCount);
	Normals.AddUninitialized(TriangleCount);

	Tangents.Empty();
	Tangents.Reserve(TriangleCount);
	Tangents.AddUninitialized(TriangleCount);

	Triangles.Empty();
	Triangles.Reserve(TriangleCount);
	Triangles.AddUninitialized(TriangleCount);
	
	
	const TArray<FFloatArray>& HeightMap = UNoise::GenerateNoiseMap(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset);

	NoiseColors.Empty();
	NoiseColors.Reserve(Rows);

	MapColors.Empty();
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

			// Noise Heights for quad vertex points
			const float BottomLeftNoiseHeight = FMath::Clamp(HeightMultiplierCurve.GetRichCurveConst()->Eval(HeightMap[X][Y]), 0.0f, 1.0f) * MeshHeightMultiplier;
			const float BottomRightNoiseHeight = FMath::Clamp(HeightMultiplierCurve.GetRichCurveConst()->Eval(HeightMap[X][Y+1]), 0.0f, 1.0f) * MeshHeightMultiplier;
			const float TopRightIndexNoiseHeight = FMath::Clamp(HeightMultiplierCurve.GetRichCurveConst()->Eval(HeightMap[X+1][Y+1]), 0.0f, 1.0f) * MeshHeightMultiplier;
			const float TopLeftIndexNoiseHeight = FMath::Clamp(HeightMultiplierCurve.GetRichCurveConst()->Eval(HeightMap[X+1][Y]), 0.0f, 1.0f) * MeshHeightMultiplier;
			
			
			// Triangle A
			const int32 BottomLeftIndex_A = VertexIndex++;
			const int32 TopRightIndex_A = VertexIndex++;
			const int32 TopLeftIndex_A = VertexIndex++;

			VertexColors[BottomLeftIndex_A] = CurrentRegionColor;
			VertexColors[TopRightIndex_A] = CurrentRegionColor;
			VertexColors[TopLeftIndex_A] = CurrentRegionColor;

			Vertices[BottomLeftIndex_A] = FVector(TopLeftX + X, TopLeftY + Y, BottomLeftNoiseHeight);
			Vertices[TopRightIndex_A] = FVector(TopLeftX + X + 1, TopLeftY + Y + 1, TopRightIndexNoiseHeight);
			Vertices[TopLeftIndex_A] = FVector(TopLeftX + X + 1, TopLeftY + Y, TopLeftIndexNoiseHeight);

			Uvs[BottomLeftIndex_A] = FVector2D(X/static_cast<float>(Rows), Y/static_cast<float>(Columns));
			Uvs[TopRightIndex_A] = FVector2D((X+1)/static_cast<float>(Rows), (Y+1)/static_cast<float>(Columns));
			Uvs[TopLeftIndex_A] = FVector2D((X+1)/static_cast<float>(Rows), Y/static_cast<float>(Columns));

			// The order of these (clockwise/counter-clockwise) dictates which way the normal will face.
			Triangles[TriangleIndex++] = BottomLeftIndex_A;
			Triangles[TriangleIndex++] = TopRightIndex_A;
			Triangles[TriangleIndex++] = TopLeftIndex_A;

			// Calculate triangle edge vectors and normal
			const FVector Edge21 = Vertices[TopRightIndex_A] - Vertices[TopLeftIndex_A];
			const FVector Edge20 = Vertices[BottomLeftIndex_A] - Vertices[TopLeftIndex_A];
			const FVector Normal_A = (Edge21 ^ Edge20).GetSafeNormal();
			
			// const FVector Normal_A = FVector::CrossProduct(Vertices[TopRightIndex_A] - Vertices[BottomLeftIndex_A], Vertices[TopLeftIndex_A] - Vertices[BottomLeftIndex_A]).GetSafeNormal();
			// const FVector Normal_A = FVector::CrossProduct(Vertices[BottomLeftIndex_A] - Vertices[TopLeftIndex_A], Vertices[TopLeftIndex_A] - Vertices[TopRightIndex_A]).GetSafeNormal();

			// If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
			Normals[BottomLeftIndex_A] = Normals[TopRightIndex_A] = Normals[TopLeftIndex_A] = Normal_A;

			// Tangents (perpendicular to the surface)
			// const FVector SurfaceTangent_A = (FVector(TopLeftX + X, TopLeftY + Y, BottomLeftNoiseHeight) - FVector(TopLeftX + X, TopLeftY + Y + 1, BottomRightNoiseHeight)).GetSafeNormal();
			const FVector SurfaceTangent_A = (Vertices[BottomLeftIndex_A] + Vertices[TopRightIndex_A] + Vertices[TopLeftIndex_A] / 3).GetSafeNormal();
			const FProcMeshTangent Tangent_A = FProcMeshTangent(SurfaceTangent_A, false);
			Tangents[BottomLeftIndex_A] = Tangents[TopRightIndex_A] = Tangents[TopLeftIndex_A] = Tangent_A;
			
			
			// Triangle B
			const int32 BottomLeftIndex_B = VertexIndex++;
			const int32 BottomRightIndex_B = VertexIndex++;
			const int32 TopRightIndex_B = VertexIndex++;

			VertexColors[BottomLeftIndex_B] = CurrentRegionColor;
			VertexColors[BottomRightIndex_B] = CurrentRegionColor;
			VertexColors[TopRightIndex_B] = CurrentRegionColor;

			Vertices[BottomLeftIndex_B] = FVector(TopLeftX + X, TopLeftY + Y, BottomLeftNoiseHeight);
			Vertices[BottomRightIndex_B] = FVector(TopLeftX + X, TopLeftY + Y + 1, BottomRightNoiseHeight);
			Vertices[TopRightIndex_B] = FVector(TopLeftX + X + 1, TopLeftY + Y + 1, TopRightIndexNoiseHeight);

			Uvs[BottomLeftIndex_B] = FVector2D(X/static_cast<float>(Rows), Y/static_cast<float>(Columns));
			Uvs[BottomRightIndex_B] = FVector2D(X/static_cast<float>(Rows), (Y+1)/static_cast<float>(Columns));
			Uvs[TopRightIndex_B] = FVector2D((X+1)/static_cast<float>(Rows), (Y+1)/static_cast<float>(Columns));

			// The order of these (clockwise/counter-clockwise) dictates which way the normal will face.
			Triangles[TriangleIndex++] = BottomLeftIndex_B;
			Triangles[TriangleIndex++] = BottomRightIndex_B;
			Triangles[TriangleIndex++] = TopRightIndex_B;

			// Calculate triangle edge vectors and normal
			const FVector Edge43 = Vertices[BottomLeftIndex_B] - Vertices[BottomRightIndex_B];
			const FVector Edge45 = Vertices[TopRightIndex_B] - Vertices[BottomRightIndex_B];
			const FVector NormalCurrent_B = (Edge43 ^ Edge45).GetSafeNormal();
			
			// const FVector NormalCurrent_B = FVector::CrossProduct(Vertices[BottomRightIndex_B] - Vertices[BottomLeftIndex_B], Vertices[TopRightIndex_B] - Vertices[BottomLeftIndex_B]).GetSafeNormal();
			// const FVector NormalCurrent_B = FVector::CrossProduct(Vertices[TopRightIndex_B] - Vertices[BottomRightIndex_B], Vertices[BottomRightIndex_B] - Vertices[BottomLeftIndex_B]).GetSafeNormal();

			// If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
			Normals[BottomLeftIndex_B] = Normals[BottomRightIndex_B] = Normals[TopRightIndex_B] = NormalCurrent_B;
			
			// Setup a quad
			
			// Tangents (perpendicular to the surface)
			// const FVector SurfaceTangent = (FVector(TopLeftX + X, TopLeftY + Y, BottomLeftNoiseHeight) - FVector(TopLeftX + X, TopLeftY + Y + 1, BottomRightNoiseHeight)).GetSafeNormal();
			const FVector SurfaceTangent_B = (Vertices[BottomLeftIndex_B] + Vertices[BottomRightIndex_B] + Vertices[TopRightIndex_B] / 3).GetSafeNormal();
			const FProcMeshTangent Tangent_B = FProcMeshTangent(SurfaceTangent_B, false);
			Tangents[BottomLeftIndex_B] = Tangents[BottomRightIndex_B] = Tangents[TopRightIndex_B] = Tangent_B;

		}

		NoiseColors.Add(NoiseColorArray);
		MapColors.Add(MapColorArray);
	}
	
	// NoiseColorsSize = NoiseColors.Num();
	// MapColorsSize = MapColors.Num();

	MeshVertexSize = Vertices.Num();
	MeshUvsSize = Uvs.Num();
	MeshTriangleSize = Triangles.Num();
	
	MeshNormalsSize = Normals.Num();
	MeshTangentsSize = Tangents.Num();
}