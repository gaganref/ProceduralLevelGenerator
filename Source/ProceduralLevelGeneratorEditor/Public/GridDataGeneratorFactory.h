// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "GridDataGeneratorFactory.generated.h"

/**
 * 
 */
UCLASS()
class PROCEDURALLEVELGENERATOREDITOR_API UGridDataGeneratorFactory : public UFactory
{
	GENERATED_BODY()

public:

	UGridDataGeneratorFactory();

	UObject* FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn);
};
