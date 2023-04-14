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
	const TArray<float>& NoiseDataNormalized = GridData->GetCurrentNoiseMapNormalized();
	
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
			const int32 CurrentIndex = UGeneratorHelpers::CoordinateToIndex(X, Y, Rows);
			const int32 CurrentSectionIndex = RegionIndexMapping[CurrentIndex];
			
			OutMeshSectionsData[CurrentSectionIndex].CreateQuad(NoiseDataNormalized, X, Y, BottomLeftX, BottomLeftY, Rows, Columns, MeshScale);
		}
	}

	return OutMeshSectionsData;
}