// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GridDataGeneratorActions.h"
#include "Modules/ModuleManager.h"

class FProceduralLevelGeneratorEditorModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

private:
	
	TSharedPtr<FGridDataGeneratorAssetTypeActions> GridDataGeneratorAssetTypeActions;	
};
