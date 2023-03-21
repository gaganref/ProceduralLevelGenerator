// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "KismetProceduralMeshLibrary.h"
#include "Structs.generated.h"

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

	void Reserve(const int32& Size)
	{
		FloatArray.Reserve(Size);
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

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector2D> Uvs;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<int32> Triangles;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<FVector> Normals;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	TArray<struct FProcMeshTangent> Tangents;

	void AddVertex(const FVector& Vertex)
	{
		Vertices.Add(Vertex);
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

	void RecalculateNormals()
	{
		Normals = CalculateNormals();
	}

	void RecalculateTangents()
	{
		UKismetProceduralMeshLibrary::CalculateTangentsForMesh(Vertices, Triangles, Uvs, Normals, Tangents);
	}

private:
	
	TArray<FVector> CalculateNormals()
	{
		TArray<FVector> VertexNormals;
		VertexNormals.Init(FVector::ZeroVector, Triangles.Num());
		const int TriangleCount = Triangles.Num()/3;

		for(int Itr=0; Itr<TriangleCount; Itr++)
		{
			const int NormalTriangleIndex = Itr*3;
			int VertexIndexA = Triangles[NormalTriangleIndex];
			int VertexIndexB = Triangles[NormalTriangleIndex + 1];
			int VertexIndexC = Triangles[NormalTriangleIndex + 2];
			
			const FVector TriangleNormal = CalculateSurfaceNormalFromIndices(VertexIndexA, VertexIndexB, VertexIndexC);
			VertexNormals[VertexIndexA] += TriangleNormal;
			VertexNormals[VertexIndexB] += TriangleNormal;
			VertexNormals[VertexIndexC] += TriangleNormal;
		}

		for(int Itr = 0; Itr<VertexNormals.Num(); Itr++)
		{
			VertexNormals[Itr] = VertexNormals[Itr].GetSafeNormal();
		}

		return VertexNormals;
	}

	FVector CalculateSurfaceNormalFromIndices(const int32& IndexA, const int32& IndexB, const int32& IndexC)
	{
		const FVector PointA = Vertices[IndexA];
		const FVector PointB = Vertices[IndexB];
		const FVector PointC = Vertices[IndexC];

		const FVector SideAb = PointB - PointA;
		const FVector SideAc = PointC - PointA;

		return FVector::CrossProduct(SideAb, SideAc).GetSafeNormal();
	}
};
