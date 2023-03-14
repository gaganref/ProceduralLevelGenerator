// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid/Grid.h"

void UGrid::PostInitProperties()
{
	Super::PostInitProperties();

	InitGrid();
}

void UGrid::PostReinitProperties()
{
	Super::PostReinitProperties();
}

#if WITH_EDITOR

void UGrid::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UGrid::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedEvent);
}

EDataValidationResult UGrid::IsDataValid(TArray<FText>& ValidationErrors)
{
	return Super::IsDataValid(ValidationErrors);
}

#endif

void UGrid::InitGrid()
{
}