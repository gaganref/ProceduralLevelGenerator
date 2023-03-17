// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "DynamicTextureComponent.generated.h"

struct FFloatArray;
struct FLinearColorArray;

// Inspired from Ref: https://gist.github.com/iUltimateLP/baca7aee4b28585b5fd2d0d46b541d95
// Adjusted the above to fulfill the requirements of the current project use


UCLASS(ClassGroup=(Custom), meta=(BlueprintSpawnableComponent))
class PROCEDURALLEVELGENERATOR_API UDynamicTextureComponent : public UActorComponent
{
	GENERATED_BODY()

private:

	bool bComponentInitialized = false;
	bool bIsInitializing = false;

	// Reference to the UTexture2D* were drawing to
	UPROPERTY()
	TObjectPtr<UTexture2D> Texture;

	int32 TextureWidth = 0;
	int32 TextureHeight = 0;
	
	// The clear color of the canvas
	FLinearColor ClearColor;

	// Unique pointer to the raw pixel data of the texture
	TUniquePtr<uint8[]> PixelBuffer;

	// Unique pointer to the proxy used to update the texture region (not really using regions,
	// it's one "big" region)
	TUniquePtr<FUpdateTextureRegion2D> UpdateTextureRegionProxy;	
	
public:
	// Sets default values for this component's properties
	UDynamicTextureComponent();

public:
	// Initializes the dynamic texture with given dimensions
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void Initialize(const int32& InWidth, const int32& InHeight, const FLinearColor& InClearColor,
												  const TArray<FLinearColorArray>& PixelColors, const TextureFilter& FilterMethod = TextureFilter::TF_Nearest);

	// Use this to ReInitializes the dynamic texture with given dimensions
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void ReInitialize(const int32& InWidth, const int32& InHeight, const FLinearColor& InClearColor,
												  const TArray<FLinearColorArray>& PixelColors, const TextureFilter& FilterMethod = TextureFilter::TF_Nearest);

	// Sets a specified pixel to a color
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void SetPixel(const int32& X, const int32& Y, const FLinearColor& Color) const;

	// Fills the texture with a given color
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void Fill(const FLinearColor& Color) const;

	// Fills the texture with a given color
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void SetAllPixels(const TArray<FLinearColorArray>& Colors) const;

	// Fills a rectangle area of the texture with a given color
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void FillRect(const int32& X, const int32& Y, const int32& Width, const int32& Height, const FLinearColor& Color) const;

	// Clears the canvas (same as filling with the clear color)
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void Clear() const;

	// Returns the UTexture resource which is used as a canvas
	UFUNCTION(BlueprintPure, Category = "Dynamic Texture")
	UTexture2D* GetTextureResource() const;

	// Needs to be called at the end of each drawing operation to update the texture
	// You can also call this at the end of multiple drawing operations, so the UTexture
	// does not get updated more than needed.
	UFUNCTION(BlueprintCallable, Category = "Dynamic Texture")
	void UpdateTexture() const;

	// Returns the width of this texture
	UFUNCTION(BlueprintPure, Category = "Dynamic Texture")
	int32 GetWidth() const;

	// Returns the height of this texture
	UFUNCTION(BlueprintPure, Category = "Dynamic Texture")
	int32 GetHeight() const;

private:
	// Internal function to set a pixel in the image
	static void SetPixelInternal(uint8*& Ptr, const uint8& Red, const uint8& Green, const uint8& Blue, const uint8& Alpha);

	// Internal function to return the pointer pointing to the specified pixel
	uint8* GetPointerToPixel(const int32& X, const int32& Y) const;

	//Internal function to Create a texture with given size
	TObjectPtr<UTexture2D> CreateTextureInternal(const int32& InWidth, const int32& InHeight, const TextureFilter& FilterMethod) const;

	bool IsValidColorArray(const TArray<FLinearColorArray>& Colors) const;

	// Fill the texture with given colors or clean it with clear color.
	void InitializePixelColorsInternal(const TArray<FLinearColorArray>& PixelColors) const;

	void MakeTextureRegionProxy(const int32& InWidth, const int32& InHeight, const bool bForceMake = false);

	void MakePixelBuffer(const int32& InWidth, const int32& InHeight);

	bool ShouldUpdateTextureSize(const int32& InWidth, const int32& InHeight) const;

	static bool ValidTextureSize(const int32& InWidth, const int32& InHeight);
};
