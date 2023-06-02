// Copyright Epic Games, Inc. All Rights Reserved.


#include "ProceduralLevelGeneratorEditor.h"

#define LOCTEXT_NAMESPACE "FProceduralLevelGeneratorEditorModule"

void FProceduralLevelGeneratorEditorModule::StartupModule()
{
	GridDataGeneratorAssetTypeActions = MakeShared<FGridDataGeneratorAssetTypeActions>();
	FAssetToolsModule::GetModule().Get().RegisterAssetTypeActions(GridDataGeneratorAssetTypeActions.ToSharedRef());
}

void FProceduralLevelGeneratorEditorModule::ShutdownModule()
{
	if (!FModuleManager::Get().IsModuleLoaded("AssetTools")) return;
	FAssetToolsModule::GetModule().Get().UnregisterAssetTypeActions(GridDataGeneratorAssetTypeActions.ToSharedRef());
}

#undef LOCTEXT_NAMESPACE
	
IMPLEMENT_MODULE(FProceduralLevelGeneratorEditorModule, ProceduralLevelGeneratorEditor)