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

	GridData->SortLevelRegions();
	
	const int32 Rows = GridData->GetRows();
	const int32 Columns = GridData->GetColumns();
	const FVector MeshScale = GridData->GetMeshScale();
	const int32 NoOfSections = GridData->GetLevelRegions().Num();
	const TArray<FFloatArray>& NoiseDataNormalized = GridData->GetCurrentNoiseMapNormalized();
	
	TArray<uint8> RegionIndexMapping;
	TArray<int32> RegionCellCount;
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


// Do not use this

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
	TArray<int32> RegionCellCount;
	GridData->CalculateRegionData(RegionIndexMapping, RegionCellCount);

	TArray<int32> VertexIndex;
	VertexIndex.Reserve(NoOfSections);
	VertexIndex.Init(0, NoOfSections);

	TArray<int32> TriangleIndex;
	TriangleIndex.Reserve(NoOfSections);
	TriangleIndex.Init(0, NoOfSections);
	
	TArray<FProcMeshSection> MeshSections;
	MeshSections.SetNum(NoOfSections, false);

	for(int32 SectionIndex = 0; SectionIndex < NoOfSections; SectionIndex++)
	{
		FProcMeshSection CurrentSection;

		// Copy data to vertex buffer
		const int32 NumVerts = RegionCellCount[SectionIndex] * 6;	// As we are using flat shading each quad(grid cell) has 6 verts
		CurrentSection.ProcVertexBuffer.Reset();
		CurrentSection.ProcVertexBuffer.AddUninitialized(NumVerts);

		CurrentSection.ProcIndexBuffer.Reset();
		CurrentSection.ProcIndexBuffer.AddUninitialized(NumVerts);
		
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
			
			int32& CurrentVertexIndex = VertexIndex[CurrentSectionIndex];
			int32& CurrentTriangleIndex = TriangleIndex[CurrentSectionIndex];
			
			FProcMeshSection& CurrentMeshSection = MeshSections[CurrentSectionIndex];
			
			CreateQuad(NoiseDataNormalized, X, Y, BottomLeftX, BottomLeftY, Rows, Columns, MeshScale, CurrentMeshSection, CurrentVertexIndex, CurrentTriangleIndex);
		}
	}

	for(int32 SectionIndex = 0; SectionIndex < NoOfSections; SectionIndex++)
	{
		MeshSections[SectionIndex].bEnableCollision = true;
		SetMaterial(SectionIndex, GridData->GetLevelRegions()[SectionIndex].RegionMaterial);
		SetProcMeshSection(SectionIndex, MeshSections[SectionIndex]);
	}
}

void UCustomProceduralMeshComponent::CreateQuad(const TArray<FFloatArray>& NoiseDataNormalized, const int32& InX,
	const int32& InY, const float& InBottomLeftX, const float& InBottomLeftY, const int32& Rows, const int32& Columns,
	FVector QuadScale, FProcMeshSection& MeshSection, int32& VertexIndex, int32& TriangleIndex)
{
	QuadScale.X = (QuadScale.X < 1) ? 1.0f : QuadScale.X;
	QuadScale.Y = (QuadScale.Y < 1) ? 1.0f : QuadScale.Y;
	QuadScale.Z = (QuadScale.Z < 1) ? 1.0f : QuadScale.Z;

	const int32 InitialVertexIndex = VertexIndex;
	
	// Triangle A - Index 0-2, Triangle A - Index 3-5
	for(int i = 0; i < 6; i++)
	{
		const int32 CurrentVertexIndex = VertexIndex++;
		FProcMeshVertex& Vertex = MeshSection.ProcVertexBuffer[CurrentVertexIndex];

		const int32 CurrX = InX + CellVertexPos[i].X;
		const int32 CurrY = InY + CellVertexPos[i].Y;
		
		Vertex.Position = FVector
		(
			InBottomLeftX + CurrX,
			InBottomLeftY + CurrY,
			NoiseDataNormalized[CurrX][CurrY]
		) * QuadScale;

		Vertex.UV0 = FVector2D(CurrX/static_cast<float>(Rows), CurrY/static_cast<float>(Columns));
		MeshSection.ProcIndexBuffer[CurrentVertexIndex] = CurrentVertexIndex;
		MeshSection.SectionLocalBox += Vertex.Position;
	}
	
	FProcMeshVertex& Vertex0 = MeshSection.ProcVertexBuffer[InitialVertexIndex];
	FProcMeshVertex& Vertex1 = MeshSection.ProcVertexBuffer[InitialVertexIndex + 1];
	FProcMeshVertex& Vertex2 = MeshSection.ProcVertexBuffer[InitialVertexIndex + 2];
	Vertex0.Normal = Vertex1.Normal = Vertex2.Normal = CalculateNormal(Vertex0, Vertex1, Vertex2);
	Vertex0.Tangent = Vertex1.Tangent = Vertex2.Tangent = CalculateTangent(Vertex0, Vertex1, Vertex2);
	
	FProcMeshVertex& Vertex3 = MeshSection.ProcVertexBuffer[InitialVertexIndex + 3];
	FProcMeshVertex& Vertex4 = MeshSection.ProcVertexBuffer[InitialVertexIndex + 4];
	FProcMeshVertex& Vertex5 = MeshSection.ProcVertexBuffer[InitialVertexIndex + 5];
	Vertex3.Normal = Vertex4.Normal = Vertex5.Normal = CalculateNormal(Vertex3, Vertex4, Vertex5);
	Vertex3.Tangent = Vertex4.Tangent = Vertex5.Tangent = CalculateTangent(Vertex3, Vertex4, Vertex5);

	
	// const int32 VertexIndex0 = VertexIndex++;
	// const int32 VertexIndex1 = VertexIndex++;
	// const int32 VertexIndex2 = VertexIndex++;
	// const int32 VertexIndex3 = VertexIndex++;
	// const int32 VertexIndex4 = VertexIndex++;
	// const int32 VertexIndex5 = VertexIndex++;
	//
	// FProcMeshVertex& Vertex0 = MeshSection.ProcVertexBuffer[VertexIndex0];
	// FProcMeshVertex& Vertex1 = MeshSection.ProcVertexBuffer[VertexIndex1];
	// FProcMeshVertex& Vertex2 = MeshSection.ProcVertexBuffer[VertexIndex2];
	//
	// FProcMeshVertex& Vertex3 = MeshSection.ProcVertexBuffer[VertexIndex3];
	// FProcMeshVertex& Vertex4 = MeshSection.ProcVertexBuffer[VertexIndex4];
	// FProcMeshVertex& Vertex5 = MeshSection.ProcVertexBuffer[VertexIndex5];
	
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
