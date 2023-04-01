// Fill out your copyright notice in the Description page of Project Settings.


#include "GridDataGeneratorActions.h"

#include "GridDataGeneratorToolkit.h"
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

void FGridDataGeneratorAssetTypeActions::OpenAssetEditor(const TArray<UObject*>& InObjects,
	TSharedPtr<IToolkitHost> EditWithinLevelEditor)
{
	const EToolkitMode::Type Mode = EditWithinLevelEditor.IsValid() ? EToolkitMode::WorldCentric : EToolkitMode::Standalone;

	for (auto ObjIt = InObjects.CreateConstIterator(); ObjIt; ++ObjIt)
	{
		if (TObjectPtr<UGridDataGenerator> PoseSearchDb = Cast<UGridDataGenerator>(*ObjIt))
		{
			const TSharedRef<FGridDataGeneratorToolkit> NewEditor(new FGridDataGeneratorToolkit());
			NewEditor->InitEditor(Mode, EditWithinLevelEditor, PoseSearchDb);
		}
	}
	
	// MakeShared<FGridDataGeneratorToolkit>()->InitEditor(InObjects);
}
