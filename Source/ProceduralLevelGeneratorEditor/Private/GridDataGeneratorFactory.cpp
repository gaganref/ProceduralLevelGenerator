// Fill out your copyright notice in the Description page of Project Settings.


#include "GridDataGeneratorFactory.h"

#include "Grid/GridDataGenerator.h"

UGridDataGeneratorFactory::UGridDataGeneratorFactory()
{
	SupportedClass = UGridDataGenerator::StaticClass();
	bCreateNew = true;
}

UObject* UGridDataGeneratorFactory::FactoryCreateNew(UClass* Class, UObject* InParent, FName Name, EObjectFlags Flags,
	UObject* Context, FFeedbackContext* Warn)
{
	return NewObject<UGridDataGenerator>(InParent, Class, Name, Flags, Context);
}
