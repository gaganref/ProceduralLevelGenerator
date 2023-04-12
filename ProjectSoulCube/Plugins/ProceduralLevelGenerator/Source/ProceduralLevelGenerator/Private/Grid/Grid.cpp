// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/Grid.h"

#include "Generator/GeneratorHelpers.h"
#include "Kismet/KismetMathLibrary.h"

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
	
	InitGrid_Triangle_Shading();
	
	// InitGrid_Quad_Shading();

	
	float TimeElapsedInMs = (FDateTime::UtcNow() - StartTime).GetTotalMilliseconds();
	float TimeElapsedInS = (FDateTime::UtcNow() - StartTime).GetTotalSeconds();
	UE_LOG(LogTemp, Error, TEXT("InitGrid - Excecution time = %f MilliSeconds = %f Seconds."), TimeElapsedInMs, TimeElapsedInS);
}

void UGrid::ClearData()
{
	NoiseColors.Empty();		// TODO: Can Improve Performance.
	MapColors.Empty();
}

void UGrid::InitGrid_Triangle_Shading()
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
	
	
	const TArray<FFloatArray>& HeightMap = UGeneratorHelpers::GenerateNoiseMap(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset);

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
	
	NoiseColorsSize = NoiseColors.Num();
	MapColorsSize = MapColors.Num();

	MeshVertexSize = Vertices.Num();
	MeshUvsSize = Uvs.Num();
	MeshTriangleSize = Triangles.Num();
	
	MeshNormalsSize = Normals.Num();
	MeshTangentsSize = Tangents.Num();
}

void UGrid::InitGrid_Quad_Shading()
{

	// const int32 TriangleCount = Rows * Columns * 2 * 3; // 2x3 vertex indexes per quad and each tri has 3 vertices and it doesn't share them
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
	Triangles.Reserve(TriangleCount);
	Triangles.AddUninitialized(TriangleCount);
	
	
	const TArray<FFloatArray>& HeightMap = UGeneratorHelpers::GenerateNoiseMap(Seed, Rows+1, Columns+1, Scale, Octaves, Persistence, Lacunarity, Offset);

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
			
			// Triangle A
			

			
			
			// Triangle B

			

			
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
	const TArray<FFloatArray>& HeightMap = UGeneratorHelpers::GenerateNoiseMap(Seed, Rows, Columns, Scale, Octaves, Persistence, Lacunarity, Offset);

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

