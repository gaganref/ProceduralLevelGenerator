// Fill out your copyright notice in the Description page of Project Settings.


#include "Generator/ObjectsGenerator.h"


// Sets default values
AObjectsGenerator::AObjectsGenerator()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AObjectsGenerator::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AObjectsGenerator::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

