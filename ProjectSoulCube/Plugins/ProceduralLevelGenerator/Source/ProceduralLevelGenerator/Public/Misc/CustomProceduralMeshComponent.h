// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ProceduralMeshComponent.h"

#include "CustomProceduralMeshComponent.generated.h"

struct FFloatArray;

UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROCEDURALLEVELGENERATOR_API UCustomProceduralMeshComponent : public UProceduralMeshComponent
{
	GENERATED_BODY()

public:

	UCustomProceduralMeshComponent() = default;
	
	// Sets default values for this component's properties
	UCustomProceduralMeshComponent(const FObjectInitializer& ObjectInitializer);

	void GenerateTerrainMesh(const TArray<FFloatArray>& HeightMap, const float& HeightMultiplier);

};
