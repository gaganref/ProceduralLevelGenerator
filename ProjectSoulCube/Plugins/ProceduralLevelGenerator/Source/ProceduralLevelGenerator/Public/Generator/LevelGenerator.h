// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "LevelGenerator.generated.h"

class UGridDataGenerator;
class UCustomProceduralMeshComponent;

UCLASS()
class PROCEDURALLEVELGENERATOR_API ALevelGenerator : public AActor
{
	GENERATED_BODY()

private:

	/** Default root scene component. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UCustomProceduralMeshComponent> ProceduralMeshComponent;

private:
	
	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bDebug = false;

private:
	
	// use this with caution
	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bGenerateOnConstruction = false;

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UGridDataGenerator> GridData;
	
public:	
	// Sets default values for this actor's properties
	ALevelGenerator() = default;

	ALevelGenerator(const FObjectInitializer& ObjectInitializer);
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual void OnConstruction(const FTransform& Transform) override;
	
protected:
	
	void GenerateMesh() const;

public:
	
	UFUNCTION(Category = "Level Data", CallInEditor)
	void GenerateLevel() const;
};
