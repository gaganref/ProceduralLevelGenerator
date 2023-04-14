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

private:
	
	static FVector Scale;

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

	static TArray<FLevelSection> GenerateMeshSectionData(const TObjectPtr<UGridDataGenerator> GridData);

};