// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KismetProceduralMeshLibrary.h"
#include "Structs.generated.h"


UENUM()
enum EMeshSection
{
	MS_WaterDeep			UMETA(DisplayName = "Water Deep"),
	MS_WaterShallow			UMETA(DisplayName = "Water Shallow"),
	MS_Sand					UMETA(DisplayName = "Sand"),
	MS_GrassLight			UMETA(DisplayName = "Grass Light"),
	MS_GrassDark			UMETA(DisplayName = "Grass Dark"),
	MS_DirtLight			UMETA(DisplayName = "Dirt Light"),
	MS_DirtDark				UMETA(DisplayName = "Dirt Dark"),
	MS_Snow					UMETA(DisplayName = "Snow"),
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
	FString TerrainName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float MinHeight;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (ClampMin=0.0f, ClampMax=1.0f))
	float MaxHeight;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FLinearColor LinearColor;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	TObjectPtr<UMaterialInstance> RegionMaterial;
	
	FTerrainType() = default;
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

	void Add(FTerrainType& Value)
	{
		float CurrentMinHeight = 0.0f;
		for(int Itr=0; Itr<Terrains.Num(); ++Itr)
		{
			if(Value.MaxHeight <= Terrains[Itr].MaxHeight)
			{
				Value.MinHeight = CurrentMinHeight;
				Terrains.Insert(Value, Itr);
			}
			else
			{
				CurrentMinHeight = Terrains[Itr].MaxHeight;
			}
		}
		Value.MinHeight = CurrentMinHeight;
		Terrains.Add(Value);
	}

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
};


/**
 * Struct that holds each cell data of the grid
 */
USTRUCT(BlueprintType)
struct PROCEDURALLEVELGENERATOR_API FCellVertices
{
	GENERATED_BODY()

	UPROPERTY(Category = "Cell Vertices Data", BlueprintReadOnly)
	FVector TopLeft;

	UPROPERTY(Category = "Cell Vertices Data", BlueprintReadOnly)
	FVector TopRight;
	
	UPROPERTY(Category = "Cell Vertices Data", BlueprintReadOnly)
	FVector BottomLeft;

	UPROPERTY(Category = "Cell Vertices Data", BlueprintReadOnly)
	FVector BottomRight;

	FCellVertices() = default;

	FCellVertices(const FVector& InTopLeft, const FVector& InTopRight, const FVector& InBottomLeft, const FVector& InBottomRight)
		: TopLeft(InTopLeft), TopRight(InTopRight), BottomLeft(InBottomLeft), BottomRight(InBottomRight) {}
};


/**
 * Struct that holds each cell data of the grid
 */
USTRUCT(BlueprintType)
struct PROCEDURALLEVELGENERATOR_API FGridCell
{
	GENERATED_BODY()

	// Location is the center of the cell
	UPROPERTY(Category = "Cell Data", BlueprintReadOnly)
	FVector Location;

	UPROPERTY(Category = "Cell Vertices Data", BlueprintReadOnly)
	FCellVertices CellVertices;

	FGridCell() = default;

	explicit FGridCell(const FVector& InLocation) : Location(InLocation) {}

	// Constructor with cell vertices
	FGridCell(const FVector& InLocation, const FVector& InTopLeft, const FVector& InTopRight, const FVector& InBottomLeft, const FVector& InBottomRight)
		: Location(InLocation),	CellVertices(InTopLeft, InTopRight, InBottomLeft, InBottomRight){}
	
};


