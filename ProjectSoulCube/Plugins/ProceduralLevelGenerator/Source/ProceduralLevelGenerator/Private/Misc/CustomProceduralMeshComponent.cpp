// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/CustomProceduralMeshComponent.h"

#include "KismetProceduralMeshLibrary.h"
#include "Grid/GridDataGenerator.h"
#include "Misc/Structs.h"

FVector UCustomProceduralMeshComponent::Scale(FVector::OneVector);

static const TArray<FLinearColor> EmptyVertexColors;

static const FVector BottomLeftPosition(0.0f, 0.0f, 0.0f);
static const FVector BottomRightPosition(0.0f, 1.0f, 0.0f);
static const FVector TopRightPosition(1.0f, 1.0f, 0.0f);
static const FVector TopLeftPosition(1.0f, 0.0f, 0.0f);

// Sets default values for this component's properties
UCustomProceduralMeshComponent::UCustomProceduralMeshComponent(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
	// ...

	SetRelativeScale3D(Scale);
	UProceduralMeshComponent::SetCollisionProfileName("BlockAll");
}

void UCustomProceduralMeshComponent::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags,
	ETeleportType Teleport)
{
	Super::OnUpdateTransform(UpdateTransformFlags, Teleport);

	UE_LOG(LogTemp, Warning, TEXT("Works"));
	if(Scale != GetRelativeScale3D())
	{
		SetRelativeScale3D(Scale);
	}
}

void UCustomProceduralMeshComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	const static FName ScaleName("RelativeScale3D");
	const FName PropertyName = PropertyChangedEvent.Property ? PropertyChangedEvent.Property->GetFName() : FName();

	if(PropertyName == ScaleName)
	{
		SetRelativeScale3D(Scale);
	}
	
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UCustomProceduralMeshComponent::CreateMesh(const TObjectPtr<UGridDataGenerator> GridData)
{
	if(!GridData)
	{
		return;
	}

	int32 SectionIndex= 0;
	for(const auto& Section : GenerateMeshSectionData(GridData))
	{
		TObjectPtr<UMaterialInstance> CurrentMaterial = GridData->GetLevelRegions()[SectionIndex].RegionMaterial;
	
		CreateMeshSection_LinearColor(SectionIndex, Section.Vertices, Section.Triangles, Section.Normals, Section.Uvs, EmptyVertexColors, Section.Tangents, true);
		SetMaterial(SectionIndex, CurrentMaterial);
	
		SectionIndex++;
	}
}

TArray<FLevelSection> UCustomProceduralMeshComponent::GenerateMeshSectionData(const TObjectPtr<UGridDataGenerator> GridData)
{
	if(!GridData)
	{
		TArray<FLevelSection> EmptyMeshSections;
		return EmptyMeshSections;
	}

	const int32 Rows = GridData->GetRows();
	const int32 Columns = GridData->GetColumns();
	const FVector MeshScale = GridData->GetMeshScale();
	const int32 NoOfSections = GridData->GetLevelRegions().Num();
	const TArray<FFloatArray>& NoiseDataNormalized = GridData->GetCurrentNoiseMapNormalized();
	
	TArray<uint8> RegionIndexMapping;
	TArray<uint32> RegionCellCount;
	GridData->CalculateRegionData(RegionIndexMapping, RegionCellCount);
	
	TArray<FLevelSection> OutMeshSectionsData;
	OutMeshSectionsData.Reserve(NoOfSections);
	
	for(int32 Itr = 0; Itr < NoOfSections; Itr++)
	{
		OutMeshSectionsData.Add(FLevelSection(RegionCellCount[Itr]));
	}

	// This is the Grid based BottomLeft XY values that are unscaled as we will be scaling them in CreateQuad
	const float BottomLeftX = (Rows - 1) / -2.0f;
	const float BottomLeftY = (Columns - 1) / -2.0f;
	
	for(int X=0; X < Rows; ++X)
	{
		for(int Y=0; Y < Columns; ++Y)
		{
			// const int32 CurrentSectionIndex = GridData->GetRegionIndex(X, Y);
			const int32 CurrentIndex = CoordinateToIndex(X, Y, Rows);
			const int32 CurrentSectionIndex = RegionIndexMapping[CurrentIndex];
			
			OutMeshSectionsData[CurrentSectionIndex].CreateQuad(NoiseDataNormalized, X, Y, BottomLeftX, BottomLeftY, Rows, Columns, MeshScale);
		}
	}

	return OutMeshSectionsData;
}

void UCustomProceduralMeshComponent::CreateTest(const TObjectPtr<UGridDataGenerator> GridData)
{
	// SCOPE_CYCLE_COUNTER(STAT_ProcMesh_CreateMeshSection);

	if(!GridData)
	{
		return;
	}

	const int32 Rows = GridData->GetRows();
	const int32 Columns = GridData->GetColumns();
	const FVector MeshScale = GridData->GetMeshScale();
	const int32 NoOfSections = GridData->GetLevelRegions().Num();
	const TArray<FFloatArray>& NoiseDataNormalized = GridData->GetCurrentNoiseMapNormalized();
	
	TArray<uint8> RegionIndexMapping;
	TArray<uint32> RegionCellCount;
	GridData->CalculateRegionData(RegionIndexMapping, RegionCellCount);
	
	TArray<FProcMeshSection> MeshSections;
	MeshSections.Reserve(NoOfSections);
	MeshSections.AddUninitialized(NoOfSections);

	for(int32 SectionIndex = 0; SectionIndex < NoOfSections; SectionIndex++)
	{
		FProcMeshSection CurrentSection;

		// Copy data to vertex buffer
		const int32 NumVerts = RegionCellCount[SectionIndex] * 6;	// As we are using flat shading each quad(grid cell) has 6 verts
		CurrentSection.ProcVertexBuffer.Reset();
		CurrentSection.ProcVertexBuffer.AddUninitialized(NumVerts);

		MeshSections[SectionIndex] = CurrentSection;
	}

	// This is the Grid based BottomLeft XY values that are unscaled as we will be scaling them in CreateQuad
	const float BottomLeftX = (Rows - 1) / -2.0f;
	const float BottomLeftY = (Columns - 1) / -2.0f;
	
	for(int X=0; X < Rows; ++X)
	{
		for(int Y=0; Y < Columns; ++Y)
		{
			const int32 CurrentIndex = CoordinateToIndex(X, Y, Rows);
			const int32 CurrentSectionIndex = RegionIndexMapping[CurrentIndex];
			
			// OutMeshSectionsData[CurrentRegionIndex].CreateQuad(NoiseDataNormalized, X, Y, BottomLeftX, BottomLeftY, Rows, Columns, MeshScale);
		}
	}
}

void UCustomProceduralMeshComponent::CreateQuad(const TArray<FFloatArray>& NoiseDataNormalized, const int32& InX,
	const int32& InY, const float& InBottomLeftX, const float& InBottomLeftY, const int32& Rows, const int32& Columns,
	FVector QuadScale)
{
	// // To Avoid 
	// QuadScale.X = (QuadScale.X < 1) ? 1.0f : QuadScale.X;
	// QuadScale.Y = (QuadScale.Y < 1) ? 1.0f : QuadScale.Y;
	// QuadScale.Z = (QuadScale.Z < 1) ? 1.0f : QuadScale.Z;
	//
	// // Noise Heights for quad vertex points
	// const float BottomLeftNoiseHeight = NoiseDataNormalized[InX][InY];
	// const float BottomRightNoiseHeight = NoiseDataNormalized[InX][InY+1];
	// const float TopRightIndexNoiseHeight = NoiseDataNormalized[InX+1][InY+1];
	// const float TopLeftIndexNoiseHeight = NoiseDataNormalized[InX+1][InY];
	//
	// // Triangle A
	// const int32 BottomLeftIndex_A = VertexIndex++;
	// const int32 TopRightIndex_A = VertexIndex++;
	// const int32 TopLeftIndex_A = VertexIndex++;
	//
	// Vertices[BottomLeftIndex_A] = FVector(InBottomLeftX + InX, InBottomLeftY + InY, BottomLeftNoiseHeight) * QuadScale;
	// Vertices[TopRightIndex_A] = FVector(InBottomLeftX + InX + 1, InBottomLeftY + InY + 1, TopRightIndexNoiseHeight) * QuadScale;
	// Vertices[TopLeftIndex_A] = FVector(InBottomLeftX + InX + 1, InBottomLeftY + InY, TopLeftIndexNoiseHeight) * QuadScale;
	//
	// Uvs[BottomLeftIndex_A] = FVector2D(InX/static_cast<float>(Rows), InY/static_cast<float>(Columns));
	// Uvs[TopRightIndex_A] = FVector2D((InX+1)/static_cast<float>(Rows), (InY+1)/static_cast<float>(Columns));
	// Uvs[TopLeftIndex_A] = FVector2D((InX+1)/static_cast<float>(Rows), InY/static_cast<float>(Columns));
	//
	// // The order of these (clockwise/counter-clockwise) dictates which way the normal will face.
	// Triangles[TriangleIndex++] = BottomLeftIndex_A;
	// Triangles[TriangleIndex++] = TopRightIndex_A;
	// Triangles[TriangleIndex++] = TopLeftIndex_A;
	//
	// // Calculate triangle edge vectors and normal
	// const FVector Edge21 = Vertices[TopRightIndex_A] - Vertices[TopLeftIndex_A];
	// const FVector Edge20 = Vertices[BottomLeftIndex_A] - Vertices[TopLeftIndex_A];
	// const FVector Normal_A = (Edge21 ^ Edge20).GetSafeNormal();
	//
	// // If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
	// Normals[BottomLeftIndex_A] = Normals[TopRightIndex_A] = Normals[TopLeftIndex_A] = Normal_A;
	//
	// // Tangents (perpendicular to the surface)
	// const FVector SurfaceTangent_A = (Vertices[BottomLeftIndex_A] + Vertices[TopRightIndex_A] + Vertices[TopLeftIndex_A] / 3).GetSafeNormal();
	// const FProcMeshTangent Tangent_A = FProcMeshTangent(SurfaceTangent_A, false);
	// Tangents[BottomLeftIndex_A] = Tangents[TopRightIndex_A] = Tangents[TopLeftIndex_A] = Tangent_A;
	//
	//
	// // Triangle B
	// const int32 BottomLeftIndex_B = VertexIndex++;
	// const int32 BottomRightIndex_B = VertexIndex++;
	// const int32 TopRightIndex_B = VertexIndex++;
	//
	// Vertices[BottomLeftIndex_B] = FVector(InBottomLeftX + InX, InBottomLeftY + InY, BottomLeftNoiseHeight) * QuadScale;
	// Vertices[BottomRightIndex_B] = FVector(InBottomLeftX + InX, InBottomLeftY + InY + 1, BottomRightNoiseHeight) * QuadScale;
	// Vertices[TopRightIndex_B] = FVector(InBottomLeftX + InX + 1, InBottomLeftY + InY + 1, TopRightIndexNoiseHeight) * QuadScale;
	//
	// Uvs[BottomLeftIndex_B] = FVector2D(InX/static_cast<float>(Rows), InY/static_cast<float>(Columns));
	// Uvs[BottomRightIndex_B] = FVector2D(InX/static_cast<float>(Rows), (InY+1)/static_cast<float>(Columns));
	// Uvs[TopRightIndex_B] = FVector2D((InX+1)/static_cast<float>(Rows), (InY+1)/static_cast<float>(Columns));
	//
	// // The order of these (clockwise/counter-clockwise) dictates which way the normal will face.
	// Triangles[TriangleIndex++] = BottomLeftIndex_B;
	// Triangles[TriangleIndex++] = BottomRightIndex_B;
	// Triangles[TriangleIndex++] = TopRightIndex_B;
	//
	// // Calculate triangle edge vectors and normal
	// const FVector Edge43 = Vertices[BottomLeftIndex_B] - Vertices[BottomRightIndex_B];
	// const FVector Edge45 = Vertices[TopRightIndex_B] - Vertices[BottomRightIndex_B];
	// const FVector NormalCurrent_B = (Edge43 ^ Edge45).GetSafeNormal();
	//
	// // If not smoothing we just set the vertex normal to the same normal as the polygon they belong to
	// Normals[BottomLeftIndex_B] = Normals[BottomRightIndex_B] = Normals[TopRightIndex_B] = NormalCurrent_B;
	//
	//
	// // Tangents (perpendicular to the surface)
	// const FVector SurfaceTangent_B = (Vertices[BottomLeftIndex_B] + Vertices[BottomRightIndex_B] + Vertices[TopRightIndex_B] / 3).GetSafeNormal();
	// const FProcMeshTangent Tangent_B = FProcMeshTangent(SurfaceTangent_B, false);
	// Tangents[BottomLeftIndex_B] = Tangents[BottomRightIndex_B] = Tangents[TopRightIndex_B] = Tangent_B;
}

void UCustomProceduralMeshComponent::CreateTriangle(FProcMeshSection& CurrentSection, FProcMeshVertex& BottomLeftVertex, FProcMeshVertex& TopRightVertex, FProcMeshVertex& TopLeftVertex,
						const TArray<FFloatArray>& NoiseDataNormalized, const int32& InX, const int32& InY, const float& InBottomLeftX,
						const float& InBottomLeftY, const int32& Rows, const int32& Columns, FVector QuadScale)
{
	QuadScale.X = (QuadScale.X < 1) ? 1.0f : QuadScale.X;
	QuadScale.Y = (QuadScale.Y < 1) ? 1.0f : QuadScale.Y;
	QuadScale.Z = (QuadScale.Z < 1) ? 1.0f : QuadScale.Z;
	
	// these are defaults not to be changed
	BottomLeftVertex.UV1 = TopRightVertex.UV1 = TopLeftVertex.UV1 = FVector2D::ZeroVector;
	BottomLeftVertex.UV2 = TopRightVertex.UV2 = TopLeftVertex.UV2 = FVector2D::ZeroVector;
	BottomLeftVertex.UV3 = TopRightVertex.UV3 = TopLeftVertex.UV3 = FVector2D::ZeroVector;
	BottomLeftVertex.Color = TopRightVertex.Color = TopLeftVertex.Color = FColor::White;
	
	// Noise Heights for quad vertex points
	const float BottomLeftNoiseHeight = NoiseDataNormalized[InX][InY];
	const float TopRightIndexNoiseHeight = NoiseDataNormalized[InX+1][InY+1];
	const float TopLeftIndexNoiseHeight = NoiseDataNormalized[InX+1][InY];
	
	BottomLeftVertex.Position = FVector(InBottomLeftX + InX, InBottomLeftY + InY, BottomLeftNoiseHeight) * QuadScale;
	TopRightVertex.Position = FVector(InBottomLeftX + InX + 1, InBottomLeftY + InY + 1, TopRightIndexNoiseHeight) * QuadScale;
	TopLeftVertex.Position = FVector(InBottomLeftX + InX + 1, InBottomLeftY + InY, TopLeftIndexNoiseHeight) * QuadScale;

	BottomLeftVertex.UV0 = FVector2D(InX/static_cast<float>(Rows), InY/static_cast<float>(Columns));
	TopRightVertex.UV0 = FVector2D((InX+1)/static_cast<float>(Rows), (InY+1)/static_cast<float>(Columns));
	TopLeftVertex.UV0 = FVector2D((InX+1)/static_cast<float>(Rows), InY/static_cast<float>(Columns));
	
	BottomLeftVertex.Normal = TopRightVertex.Normal = TopLeftVertex.Normal = CalculateNormal(BottomLeftVertex, TopRightVertex, TopLeftVertex);
	BottomLeftVertex.Tangent = TopRightVertex.Tangent = TopLeftVertex.Tangent = CalculateTangent(BottomLeftVertex, TopRightVertex, TopLeftVertex);
	
	// The order of these (clockwise/counter-clockwise) dictates which way the normal will face.
	// Triangles[TriangleIndex++] = BottomLeftIndex_A;
	// Triangles[TriangleIndex++] = TopRightIndex_A;
	// Triangles[TriangleIndex++] = TopLeftIndex_A;
	
	// Update bounding box
	CurrentSection.SectionLocalBox += BottomLeftVertex.Position;
	CurrentSection.SectionLocalBox += TopRightVertex.Position;
	CurrentSection.SectionLocalBox += TopLeftVertex.Position;
}

void UCustomProceduralMeshComponent::SetProcMeshVertexDefaultValues(FProcMeshVertex& VertexA, FProcMeshVertex& VertexB, FProcMeshVertex& VertexC)
{
	// these are defaults not to be changed
	VertexA.UV1 = VertexB.UV1 = VertexC.UV1 = FVector2D::ZeroVector;
	VertexA.UV2 = VertexB.UV2 = VertexC.UV2 = FVector2D::ZeroVector;
	VertexA.UV3 = VertexB.UV3 = VertexC.UV3 = FVector2D::ZeroVector;
	VertexA.Color = VertexB.Color = VertexC.Color = FColor::White;
}

FVector UCustomProceduralMeshComponent::CalculateNormal(const FProcMeshVertex& Vertex0, const FProcMeshVertex& Vertex1, const FProcMeshVertex& Vertex2)
{
	// Calculate triangle edge vectors and normal
	const FVector Edge21 = Vertex1.Position - Vertex2.Position;
	const FVector Edge20 = Vertex0.Position - Vertex2.Position;
	return (Edge21 ^ Edge20).GetSafeNormal();
}

FProcMeshTangent UCustomProceduralMeshComponent::CalculateTangent(const FProcMeshVertex& Vertex0, const FProcMeshVertex& Vertex1, const FProcMeshVertex& Vertex2)
{
	// Tangents (perpendicular to the surface)
	const FVector SurfaceTangent = (Vertex0.Position + Vertex1.Position + Vertex2.Position / 3).GetSafeNormal();
	return FProcMeshTangent(SurfaceTangent, false);
}

int32 UCustomProceduralMeshComponent::CoordinateToIndex(const int32 InX, const int32 InY, const int32 Rows)
{
	return InY * Rows +InX;
}
