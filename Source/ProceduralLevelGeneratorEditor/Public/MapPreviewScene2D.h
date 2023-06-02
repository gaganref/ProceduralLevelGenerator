// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AdvancedPreviewScene.h"

class FGridDataGeneratorToolkit;
/**
 * 
 */
class PROCEDURALLEVELGENERATOREDITOR_API FMapPreviewScene2D : public FAdvancedPreviewScene
{
	
public:
	
	FMapPreviewScene2D(ConstructionValues Cvs, const TSharedRef<FGridDataGeneratorToolkit>& EditorToolkit);
	virtual ~FMapPreviewScene2D() override;

	virtual void Tick(float InDeltaTime) override;

	TSharedRef<FGridDataGeneratorToolkit> GetEditor() const
	{
		return EditorPtr.Pin().ToSharedRef();
	}
	
private:
	
	TWeakPtr<FGridDataGeneratorToolkit> EditorPtr;
};
