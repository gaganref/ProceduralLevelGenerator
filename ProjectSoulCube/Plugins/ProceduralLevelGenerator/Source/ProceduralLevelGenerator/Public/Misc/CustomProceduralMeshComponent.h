// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"

#include "CustomProceduralMeshComponent.generated.h"

class UGridDataGenerator;
struct FLevelSection;
struct FFloatArray;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROCEDURALLEVELGENERATOR_API UCustomProceduralMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

protected:
	
	static FVector Scale;

	UPROPERTY()
	int32 NoOfVertices = 0;		// As we are using flat shading no of vertices = no of triangles = no of uvs = no of normals = no of tangents;

	

public:

	UCustomProceduralMeshComponent() = default;
	
	// Sets default values for this component's properties
	explicit UCustomProceduralMeshComponent(const FObjectInitializer& ObjectInitializer);

	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport) override;

protected:

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif

public:

	void CreateMesh(TObjectPtr<UGridDataGenerator> GridData);
	
	TArray<FLevelSection> GenerateMeshSectionData(const TObjectPtr<UGridDataGenerator> GridData);

	void CreateTest(const TObjectPtr<UGridDataGenerator> GridData);

private:

	void CreateQuad(const TArray<FFloatArray>& NoiseDataNormalized, const int32& InX, const int32& InY, const float& InBottomLeftX, const float& InBottomLeftY, const int32& Rows, const int32& Columns, FVector QuadScale);

	void CreateTriangle(FProcMeshSection& CurrentSection, FProcMeshVertex& BottomLeftVertex, FProcMeshVertex& TopRightVertex, FProcMeshVertex& TopLeftVertex,
						const TArray<FFloatArray>& NoiseDataNormalized, const int32& InX, const int32& InY, const float& InBottomLeftX,
						const float& InBottomLeftY, const int32& Rows, const int32& Columns, FVector QuadScale);

	static void SetProcMeshVertexDefaultValues(FProcMeshVertex& VertexA, FProcMeshVertex& VertexB, FProcMeshVertex& VertexC);

	static FVector CalculateNormal(const FProcMeshVertex& Vertex0, const FProcMeshVertex& Vertex1, const FProcMeshVertex& Vertex2);
	static FProcMeshTangent CalculateTangent(const FProcMeshVertex& Vertex0, const FProcMeshVertex& Vertex1, const FProcMeshVertex& Vertex2);
	
	FORCEINLINE static int32 CoordinateToIndex(int32 InX, int32 InY, int32 Rows);
};