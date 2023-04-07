// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ObjectGeneratorBase.h"
#include "TreesGenerator.generated.h"

UCLASS()
class PROCEDURALLEVELGENERATOR_API ATreesGenerator : public AObjectGeneratorBase
{
	GENERATED_BODY()

private:

	/** Default root scene component. */
	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<USceneComponent> SceneComponent;

	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInstancedStaticMeshComponent> SimpleTree;

	UPROPERTY(Category = "ActorComponent", VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UInstancedStaticMeshComponent> XmasTree;

private:
	
	UPROPERTY(Category = "Debug", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	bool bDebug = false;

private:

	UPROPERTY(Category = "Level Data", EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	TObjectPtr<class UGridDataGenerator> GridData;	
	
public:
	
	// Sets default values for this actor's properties
	ATreesGenerator();

protected:
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	
	UFUNCTION(Category = "Level Data", CallInEditor)
	void GenerateTrees() const;
	
};
