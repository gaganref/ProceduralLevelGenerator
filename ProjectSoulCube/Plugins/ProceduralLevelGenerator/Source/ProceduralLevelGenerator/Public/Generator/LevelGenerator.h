// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

struct FFloatArray;

UCLASS()
class PROCEDURALLEVELGENERATOR_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

private:

	/** Default root scene component. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;

#if WITH_EDITORONLY_DATA	
	/** Plane where the perlin noise is projected using texture. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> NoisePlane;

	/** Plane where the generated map is projected using texture. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UStaticMeshComponent> MapPlane;

	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bDebug = false;

	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bShowNoisePlane = true;

	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bShowMapPlane = true;
#endif
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator();
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	virtual void PostActorCreated() override;
	virtual void PostLoad() override;

protected:
	void InitLevelGenerator();

	void GenerateMapsOnPlanes(const TArray<FFloatArray>& NoiseMap);
	
public:

#if WITH_EDITOR	
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetNoisePlane() const {return NoisePlane;}
	FORCEINLINE TObjectPtr<UStaticMeshComponent> GetMapPlane() const {return MapPlane;}
#endif

	TObjectPtr<UTexture2D> GenerateTextureFromNoise(const TArray<FFloatArray>& NoiseMap);

public:
	// Total no of rows(→) or total no of cells(■) in each row(→) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 Rows = 10;

	// Total no of columns(↑) or total no of cells(■) in each column(↑) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 Columns = 10;

	// Size of each individual cell (as a cell is a square SizeOnX = SizeOnY)
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 0.00001f))
	float Scale = 0.2f;
};
