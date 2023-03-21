// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/CustomProceduralMeshComponent.h"

#include "KismetProceduralMeshLibrary.h"
#include "Misc/Structs.h"


// Sets default values for this component's properties
UCustomProceduralMeshComponent::UCustomProceduralMeshComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UCustomProceduralMeshComponent::GenerateTerrainMesh(const TArray<FFloatArray>& HeightMap, const float& HeightMultiplier)
{
	// const int32 MapX = HeightMap[0].Num();
	// const int32 MapY = HeightMap.Num();
	const int32 MapX = HeightMap.Num();
	const int32 MapY = HeightMap[0].Num();

	const float TopLeftX = (MapX - 1) / -2.0f;
	const float TopLeftY = (MapY - 1) / -2.0f;

	FCustomMeshData MeshData;
	int32 VertexIndex = 0;
	
	for(int X=0; X < MapX; ++X)
	{
		for(int Y=0; Y < MapY; ++Y)
		{
			MeshData.AddVertex(FVector(TopLeftX + X, TopLeftY + Y, HeightMap[X][Y] * HeightMultiplier));
			MeshData.AddUv(FVector2D(X/static_cast<float>(MapX), Y/static_cast<float>(MapY)));

			if((X < MapX-1) && (Y < MapY-1))
			{
				MeshData.AddTriangle(VertexIndex, VertexIndex + MapX + 1, VertexIndex + MapX);
				MeshData.AddTriangle(VertexIndex + MapX + 1, VertexIndex, VertexIndex + 1);
			}

			++VertexIndex;
		}
	}

	TArray<FLinearColor> TempColorArray;
	TempColorArray.Init(FLinearColor::Red, MeshData.Vertices.Num());
	MeshData.RecalculateNormals();
	MeshData.RecalculateTangents();

	CreateMeshSection_LinearColor(0, MeshData.Vertices, MeshData.Triangles, MeshData.Normals, MeshData.Uvs, TempColorArray, MeshData.Tangents, true);
}
