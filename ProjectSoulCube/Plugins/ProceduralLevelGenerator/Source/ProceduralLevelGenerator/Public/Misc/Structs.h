// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KismetProceduralMeshLibrary.h"
#include "Structs.generated.h"


UENUM(BlueprintType)
enum ELevelRegion
{
	Lr_Default				UMETA(DisplayName = "Default Region"),
	Lr_WaterDeep			UMETA(DisplayName = "Water Deep"),
	Lr_WaterShallow			UMETA(DisplayName = "Water Shallow"),
	Lr_Sand					UMETA(DisplayName = "Sand"),
	Lr_GrassLight			UMETA(DisplayName = "Grass Light"),
	Lr_GrassDark			UMETA(DisplayName = "Grass Dark"),
	Lr_DirtLight			UMETA(DisplayName = "Dirt Light"),
	Lr_DirtDark				UMETA(DisplayName = "Dirt Dark"),
	Lr_Snow					UMETA(DisplayName = "Snow"),
};

USTRUCT(BlueprintType)
struct FInt8Array
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<uint8> Array;

public:	
	FInt8Array() = default;

	explicit FInt8Array(const TArray<uint8>& InArray) : Array(InArray) {}
	
	uint8& operator[] (const int32& Index)
	{
		return Array[Index];
	}

	const uint8& operator[] (const int32& Index) const
	{
		return Array[Index];
	}

	void Init(const uint8& Element, const int32& Size)
	{
		Array.Init(Element, Size);
	}
	
	void Add(const uint8& Value)
	{
		Array.Add(Value);
	}

	void AddUninitialized(const int32& Size)
	{
		Array.AddUninitialized(Size);
	}

	void Reserve(const int32& Size)
	{
		Array.Reserve(Size);
	}

	void Empty()
	{
		Array.Empty();
	}
	
	void Reset(const int32& Size)	// Same as empty, but doesn't change memory allocations
	{
		Array.Reset(Size);
	}

	int32 Num() const
	{
		return Array.Num();
	}
};

USTRUCT(BlueprintType)
struct FInt32Array
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<int32> Array;

public:	
	FInt32Array() = default;

	explicit FInt32Array(const TArray<int32>& InArray) : Array(InArray) {}
	
	int32& operator[] (const int32& Index)
	{
		return Array[Index];
	}

	const int32& operator[] (const int32& Index) const
	{
		return Array[Index];
	}

	void Init(const int32& Element, const int32& Size)
	{
		Array.Init(Element, Size);
	}
	
	void Add(const int32& Value)
	{
		Array.Add(Value);
	}

	void AddUninitialized(const int32& Size)
	{
		Array.AddUninitialized(Size);
	}

	void Reserve(const int32& Size)
	{
		Array.Reserve(Size);
	}

	void Empty()
	{
		Array.Empty();
	}
	
	void Reset(const int32& Size)	// Same as empty, but doesn't change memory allocations
	{
		Array.Reset(Size);
	}

	int32 Num() const
	{
		return Array.Num();
	}
};

USTRUCT(BlueprintType)
struct FFloatArray
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<float> FloatArray;

public:	
	FFloatArray() = default;

	explicit FFloatArray(const TArray<float>& InFloatArray) : FloatArray(InFloatArray) {}
	
	float& operator[] (const int32& Index)
	{
		return FloatArray[Index];
	}

	const float& operator[] (const int32& Index) const
	{
		return FloatArray[Index];
	}

	void Init(const float& Element, const int32& Size)
	{
		FloatArray.Init(Element, Size);
	}
	
	void Add(const float& Value)
	{
		FloatArray.Add(Value);
	}

	void AddUninitialized(const int32& Size)
	{
		FloatArray.AddUninitialized(Size);
	}

	void Reserve(const int32& Size)
	{
		FloatArray.Reserve(Size);
	}

	void Empty()
	{
		FloatArray.Empty();
	}
	
	void Reset(const int32& Size)	// Same as empty, but doesn't change memory allocations
	{
		FloatArray.Reset(Size);
	}

	int32 Num() const
	{
		return FloatArray.Num();
	}
};

USTRUCT(BlueprintType)
struct FLinearColorArray
{
	GENERATED_BODY()

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TArray<FLinearColor> LinearColorArray;

public:	
	FLinearColorArray() = default;

	explicit FLinearColorArray(const TArray<FLinearColor>& InFloatArray) : LinearColorArray(InFloatArray) {}
	
	FLinearColor& operator[] (const int32& Index)
	{
		return LinearColorArray[Index];
	}

	const FLinearColor& operator[] (const int32& Index) const
	{
		return LinearColorArray[Index];
	}

	void Init(const FLinearColor& Element, const int32& Size)
	{
		LinearColorArray.Init(Element, Size);
	}

	void Reserve(const int32& Size)
	{
		LinearColorArray.Reserve(Size);
	}
	
	void Add(const FLinearColor& Value)
	{
		LinearColorArray.Add(Value);
	}

	void AddUninitialized(const int32& Size)
	{
		LinearColorArray.AddUninitialized(Size);
	}

	void Empty()
	{
		LinearColorArray.Empty();
	}
	
	void Reset(const int32& Size)	// Same as empty, but doesn't change memory allocations
	{
		LinearColorArray.Reset(Size);
	}

	int32 Num() const
	{
		return LinearColorArray.Num();
	}
};

USTRUCT(BlueprintType)
struct FTerrainType
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FName TerrainName = "Terrain Name";
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin=0.0f, ClampMax=1.0f))
	float MaxHeight = 0.0f;

	// Only used if the corresponding point has a valid terrain height
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin=0, ClampMax=100))
	uint8 ObjectSpawnProbability = 0;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LinearColor = FLinearColor::Black;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> RegionMaterial = nullptr;
	
	FTerrainType() = default;
	
	FORCEINLINE bool operator<(const FTerrainType& Other) const
	{
		return MaxHeight < Other.MaxHeight;
	}

	FORCEINLINE bool operator>(const FTerrainType& Other) const
	{
		return MaxHeight > Other.MaxHeight;
	}
};

USTRUCT(BlueprintType)
struct FTerrainTypeArray
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TArray<FTerrainType> Terrains;

	FTerrainType& operator[] (const int32& Index)
	{
		return Terrains[Index];
	}

	const FTerrainType& operator[] (const int32& Index) const
	{
		return Terrains[Index];
	}

	// void Add(FTerrainType& Value)
	// {
	// 	float CurrentMinHeight = 0.0f;
	// 	for(int Itr=0; Itr<Terrains.Num(); ++Itr)
	// 	{
	// 		if(Value.MaxHeight <= Terrains[Itr].MaxHeight)
	// 		{
	// 			Value.MinHeight = CurrentMinHeight;
	// 			Terrains.Insert(Value, Itr);
	// 		}
	// 		else
	// 		{
	// 			CurrentMinHeight = Terrains[Itr].MaxHeight;
	// 		}
	// 	}
	// 	Value.MinHeight = CurrentMinHeight;
	// 	Terrains.Add(Value);
	// }

	int32 Num() const
	{
		return Terrains.Num();
	}
};

USTRUCT(BlueprintType)
struct FCustomMeshData
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector> Vertices;
	
	UPROPERTY()
	TArray<FLinearColor> VertexColors;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector2D> Uvs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<int32> Triangles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector> Normals;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<struct FProcMeshTangent> Tangents;

	FCustomMeshData() = default;

	FCustomMeshData(const int32& InWidth, const int32& InHeight)
	{
		// Vertices.Reserve((InWidth - 1) * (InHeight - 1) * 6);
		// VertexColors.Reserve((InWidth - 1) * (InHeight - 1) * 6);
		// Uvs.Reserve((InWidth - 1) * (InHeight - 1) * 6);
		Vertices.Reserve(InWidth*InHeight);
		VertexColors.Reserve(InWidth*InHeight);
		Uvs.Reserve(InWidth * InHeight);
		Triangles.Reserve((InWidth - 1) * (InHeight - 1) * 6);
	}
	
	void AddVertex(const FVector& Vertex)
	{
		Vertices.Add(Vertex);
	}

	void AddVertexColor(const FLinearColor& Color)
	{
		VertexColors.Add(Color);
	}

	void AddUv(const FVector2D& Uv)
	{
		Uvs.Add(Uv);
	}
	
	void AddTriangle(const int32& A, const int32& B, const int32& C)
	{
		Triangles.Add(A);
		Triangles.Add(B);
		Triangles.Add(C);
	}

	void RecalculateNormalsAndTangents()
	{
		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, Uvs, Normals, Tangents);
	}

	void FlatShading()
	{
		TArray<FVector> FlatShadedVertices;
		TArray<FVector2D> FlatShadedUvs;
		TArray<FLinearColor> FlatShadedVertexColors;

		const int32 TrianglesLength = Triangles.Num(); 
		
		FlatShadedVertices.Reserve(TrianglesLength);
		FlatShadedUvs.Reserve(TrianglesLength);
		FlatShadedVertexColors.Reserve(TrianglesLength);

		for(int Itr=0; Itr<TrianglesLength; Itr++)
		{
			FlatShadedVertices.Add(Vertices[Triangles[Itr]]);
			FlatShadedUvs.Add(Uvs[Triangles[Itr]]);
			FlatShadedVertexColors.Add(VertexColors[Triangles[Itr]]);
			Triangles[Itr] = Itr;
		}

		Vertices = FlatShadedVertices;
		Uvs = FlatShadedUvs;
		VertexColors = FlatShadedVertexColors;
	}
};


USTRUCT(BlueprintType)
struct FCell
{
	GENERATED_BODY()

	UPROPERTY()
	int32 X;

	UPROPERTY()
	int32 Y;

	UPROPERTY()
	int32 RegionIndex;

	FCell() = default;

	FCell(const int32& InX, const int32& InY, const int32& InRegionIndex)
		: X(InX), Y(InY), RegionIndex(InRegionIndex)
	{
		
	}
};

// Uses a lots of storage needs lots of optimization or only 
USTRUCT()
struct FLevelSection
{
	GENERATED_BODY()

	UPROPERTY()
	int32 Size = 0;
	
	UPROPERTY()
	int32 VertexIndex = 0;

	UPROPERTY()
	int32 TriangleIndex = 0;
	
	UPROPERTY()
	TArray<FVector> Vertices;

	UPROPERTY()
	TArray<FVector2D> Uvs;

	UPROPERTY()
	TArray<int32> Triangles;

	UPROPERTY()
	TArray<FVector> Normals;
	
	UPROPERTY()
	TArray<struct FProcMeshTangent> Tangents;

	FLevelSection() = default;
	
	explicit FLevelSection(/** In No of sections in the cell */const int32& InSize)
		: Size(InSize * 6)
	{
		Empty_Internal();
		AddUninitialized_Internal();
	}

	void Empty_Internal()
	{
		Vertices.Empty();
		Uvs.Empty();
		Triangles.Empty();
		Normals.Empty();
		Tangents.Empty();
	}

	void AddUninitialized_Internal()
	{
		Vertices.Reserve(Size);
		Vertices.AddUninitialized(Size);

		Uvs.Reserve(Size);
		Uvs.AddUninitialized(Size);

		Triangles.Reserve(Size);
		Triangles.AddUninitialized(Size);

		Normals.Reserve(Size);
		Normals.AddUninitialized(Size);
		
		Tangents.Reserve(Size);
		Tangents.AddUninitialized(Size);
	}

	void CreateQuad(const TArray<FFloatArray>& NoiseDataNormalized, const int32& InX, const int32& InY, const float& InBottomLeftX, const float& InBottomLeftY, const int32& Rows, const int32& Columns, FVector QuadScale)
	{
		// To Avoid 
		QuadScale.X = (QuadScale.X < 1) ? 1.0f : QuadScale.X;
		QuadScale.Y = (QuadScale.Y < 1) ? 1.0f : QuadScale.Y;
		QuadScale.Z = (QuadScale.Z < 1) ? 1.0f : QuadScale.Z;
		
		// Noise Heights for quad vertex points
		const float BottomLeftNoiseHeight = NoiseDataNormalized[InX][InY];
		const float BottomRightNoiseHeight = NoiseDataNormalized[InX][InY+1];
		const float TopRightIndexNoiseHeight = NoiseDataNormalized[InX+1][InY+1];
		const float TopLeftIndexNoiseHeight = NoiseDataNormalized[InX+1][InY];
		
		// Triangle A
		const int32 BottomLeftIndex_A = VertexIndex++;
		const int32 TopRightIndex_A = VertexIndex++;
		const int32 TopLeftIndex_A = VertexIndex++;

		Vertices[BottomLeftIndex_A] = FVector(InBottomLeftX + InX, InBottomLeftY + InY, BottomLeftNoiseHeight) * QuadScale;
		Vertices[TopRightIndex_A] = FVector(InBottomLeftX + InX + 1, InBottomLeftY + InY + 1, TopRightIndexNoiseHeight) * QuadScale;
		Vertices[TopLeftIndex_A] = FVector(InBottomLeftX + InX + 1, InBottomLeftY + InY, TopLeftIndexNoiseHeight) * QuadScale;

		Uvs[BottomLeftIndex_A] = FVector2D(InX/static_cast<float>(Rows), InY/static_cast<float>(Columns));
		Uvs[TopRightIndex_A] = FVector2D((InX+1)/static_cast<float>(Rows), (InY+1)/static_cast<float>(Columns));
		Uvs[TopLeftIndex_A] = FVector2D((InX+1)/static_cast<float>(Rows), InY/static_cast<float>(Columns));

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

		Vertices[BottomLeftIndex_B] = FVector(InBottomLeftX + InX, InBottomLeftY + InY, BottomLeftNoiseHeight) * QuadScale;
		Vertices[BottomRightIndex_B] = FVector(InBottomLeftX + InX, InBottomLeftY + InY + 1, BottomRightNoiseHeight) * QuadScale;
		Vertices[TopRightIndex_B] = FVector(InBottomLeftX + InX + 1, InBottomLeftY + InY + 1, TopRightIndexNoiseHeight) * QuadScale;

		Uvs[BottomLeftIndex_B] = FVector2D(InX/static_cast<float>(Rows), InY/static_cast<float>(Columns));
		Uvs[BottomRightIndex_B] = FVector2D(InX/static_cast<float>(Rows), (InY+1)/static_cast<float>(Columns));
		Uvs[TopRightIndex_B] = FVector2D((InX+1)/static_cast<float>(Rows), (InY+1)/static_cast<float>(Columns));

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
		
		
		// Tangents (perpendicular to the surface)
		const FVector SurfaceTangent_B = (Vertices[BottomLeftIndex_B] + Vertices[BottomRightIndex_B] + Vertices[TopRightIndex_B] / 3).GetSafeNormal();
		const FProcMeshTangent Tangent_B = FProcMeshTangent(SurfaceTangent_B, false);
		Tangents[BottomLeftIndex_B] = Tangents[BottomRightIndex_B] = Tangents[TopRightIndex_B] = Tangent_B;
	}
};
