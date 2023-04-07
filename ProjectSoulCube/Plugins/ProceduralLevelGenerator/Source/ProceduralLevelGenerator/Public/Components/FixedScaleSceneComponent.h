// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/SceneComponent.h"
#include "FixedScaleSceneComponent.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURALLEVELGENERATOR_API UFixedScaleSceneComponent : public USceneComponent
{
	GENERATED_BODY()

public:

	UFixedScaleSceneComponent();

protected:

	virtual void OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport) override;

#if WITH_EDITOR

	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	
#endif	
};
