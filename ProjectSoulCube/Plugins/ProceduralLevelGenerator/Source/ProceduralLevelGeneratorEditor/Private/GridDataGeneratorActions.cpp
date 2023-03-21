// Fill out your copyright notice in the Description page of Project Settings.


#include "GridDataGeneratorActions.h"

#include "Grid/GridDataGenerator.h"

UClass* FGridDataGeneratorAssetTypeActions::GetSupportedClass() const
{
	return UGridDataGenerator::StaticClass();
}

FText FGridDataGeneratorAssetTypeActions::GetName() const
{
	return INVTEXT("Grid Data Generator");
}

FColor FGridDataGeneratorAssetTypeActions::GetTypeColor() const
{
	return FColor::Emerald;
}

uint32 FGridDataGeneratorAssetTypeActions::GetCategories()
{
	return EAssetTypeCategories::Misc;
}
