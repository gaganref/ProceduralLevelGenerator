// Fill out your copyright notice in the Description page of Project Settings

#include "Misc/Noise.h"

#define DEBUG_GET_CURR_CLASS_FUNC (FString(__FUNCTION__))
#define DEBUG_GET_CURR_LINE (FString::FromInt(__LINE__))

#define DEBUG_LOG_SIMPLE UE_LOG(LogTemp, Error, TEXT("Debug- Simple log."))
#define DEBUG_MESSAGE_SIMPLE if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Debug- Simple message.")))
#define DEBUG_PRINT_SIMPLE DEBUG_LOG_SIMPLE DEBUG_MESSAGE_SIMPLE

#define DEBUG_LOG_SIMPLE_WITH_INFO UE_LOG(LogTemp, Error, TEXT("Debug- Simple log with info. || Called at- %s::%s."), *DEBUG_GET_CURR_CLASS_FUNC, *DEBUG_GET_CURR_LINE)
#define DEBUG_MESSAGE_SIMPLE_WITH_INFO if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Debug- Simple message with info. || Called at- %s::%s."), *DEBUG_GET_CURR_CLASS_FUNC, *DEBUG_GET_CURR_LINE))
#define DEBUG_PRINT_SIMPLE_WITH_INFO DEBUG_LOG_SIMPLE_WITH_INFO DEBUG_MESSAGE_SIMPLE_WITH_INFO

#define DEBUG_LOG_CUSTOM_TEXT(text) UE_LOG(LogTemp, Error, TEXT("Debug- %s"), *FString(text))
#define DEBUG_MESSAGE_CUSTOM_TEXT(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Debug - %s"), *FString(text)))
#define DEBUG_PRINT_CUSTOM_TEXT(text) DEBUG_LOG_CUSTOM_TEXT(text) DEBUG_MESSAGE_CUSTOM_TEXT(text)

#define DEBUG_LOG_CUSTOM_TEXT_WITH_INFO(text) UE_LOG(LogTemp, Error, TEXT("Debug- %s || Called at- %s::%s."), *FString(text), *DEBUG_GET_CURR_CLASS_FUNC, *DEBUG_GET_CURR_LINE)
#define DEBUG_MESSAGE_CUSTOM_TEXT_WITH_INFO(text) if (GEngine) GEngine->AddOnScreenDebugMessage(-1, 2, FColor::Red, FString::Printf(TEXT("Debug - %s || Called at- %s::%s."), *FString(text), *DEBUG_GET_CURR_CLASS_FUNC, *DEBUG_GET_CURR_LINE))
#define DEBUG_PRINT_CUSTOM_TEXT_WITH_INFO(text) DEBUG_LOG_CUSTOM_TEXT_WITH_INFO(text) DEBUG_MESSAGE_CUSTOM_TEXT_WITH_INFO(text)

UNoise::UNoise(const FObjectInitializer& ObjectInitializer)
: Super(ObjectInitializer)
{

}

TArray<FFloatArray> UNoise::GenerateNoiseMap(const int32& MapX, const int32& MapY, float Scale)
{
	TArray<FFloatArray> NoiseMap;

	// Make Sure the scale is greater than zero to avoid zero division error.
	if(Scale <= 0)
	{
		Scale = 0.0001f;
	}
	
	for(int Y=0; Y < MapY; Y++)
	{
		FFloatArray CurrArray;
		for(int X=0; X < MapX; X++)
		{
			FVector2D Sample = FVector2D(X, Y);
			Sample /= Scale;

			float PerlinValue = FMath::PerlinNoise2D(Sample);
			CurrArray.Add(PerlinValue);

			DEBUG_PRINT_CUSTOM_TEXT(FString::SanitizeFloat(PerlinValue));
		}
		NoiseMap.Add(CurrArray);
	}
	
	return NoiseMap;
}
