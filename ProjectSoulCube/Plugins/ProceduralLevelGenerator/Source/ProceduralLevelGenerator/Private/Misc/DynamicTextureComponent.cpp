// Fill out your copyright notice in the Description page of Project Settings.


#include "Misc/DynamicTextureComponent.h"
#include "Misc/Structs.h"

static TArray<FLinearColorArray> EmptyLinearColorArray;

// UTextures have a BPP of 4 (Red, Green, Blue, Alpha)
#define DYNAMIC_TEXTURE_BYTES_PER_PIXEL 4


// Sets default values for this component's properties
UDynamicTextureComponent::UDynamicTextureComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	// ...
}

void UDynamicTextureComponent::Initialize(const int32& InWidth, const int32& InHeight, const FLinearColor& InClearColor,
                                          const TArray<FLinearColorArray>& PixelColors, const TextureFilter& FilterMethod)
{
	if(bComponentInitialized)
	{
		ReInitialize
		(
			InWidth,
			InHeight,
			InClearColor,
			PixelColors,
			FilterMethod
		);
		return;	
	}
	
	// Dont run if texture dimensions are not valid or it is currently initializing.
	if(bIsInitializing)
	{
		return;
	}
	bIsInitializing = true;
	
	TextureWidth = InWidth;
	TextureHeight = InHeight;
	ClearColor = InClearColor;
	
	Texture = CreateTextureInternal(TextureWidth, TextureHeight, FilterMethod);
	
	MakeTextureRegionProxy(TextureWidth, TextureHeight, true);
	MakePixelBuffer(TextureWidth, TextureHeight);
	InitializePixelColorsInternal(PixelColors);
	
	bIsInitializing = false;
	bComponentInitialized = true;
}

void UDynamicTextureComponent::ReInitialize(const int32& InWidth, const int32& InHeight, const FLinearColor& InClearColor,
												  const TArray<FLinearColorArray>& PixelColors, const TextureFilter& FilterMethod)
{
	if(!bComponentInitialized)
	{
		Initialize
		(
			InWidth,
			InHeight,
			InClearColor,
			PixelColors,
			FilterMethod
		);
		return;	
	}
	
	// Dont run if texture dimensions are not valid or it is currently initializing.
	if(bIsInitializing)
	{
		return;
	}
	bIsInitializing = true;
	
	// Do not re initialize the texture if it's size is not changed.
	if((TextureWidth == InWidth) && (TextureHeight == InHeight))
	{
		ClearColor = InClearColor;
		// Texture->Filter = FilterMethod;	// use this if required.
		InitializePixelColorsInternal(PixelColors);
		bIsInitializing = false;
		return;
	}
	
	TextureWidth = InWidth;
	TextureHeight = InHeight;
	ClearColor = InClearColor;
	
	Texture = CreateTextureInternal(TextureWidth, TextureHeight, FilterMethod);
	
	MakeTextureRegionProxy(TextureWidth, TextureHeight);
	MakePixelBuffer(TextureWidth, TextureHeight);
	InitializePixelColorsInternal(PixelColors);
	
	bIsInitializing = false;
}

void UDynamicTextureComponent::SetPixel(const int32& X, const int32& Y, const FLinearColor& Color) const
{
	// Get the pointer of the specified pixel
	uint8* Ptr = GetPointerToPixel(X, Y);

	// Set the pixel (note that linear color uses floats between 0..1, but a uint8 ranges from 0..255)
	SetPixelInternal(Ptr, Color.R * 255, Color.G * 255, Color.B * 255, Color.A * 255);

	// UpdateTexture();
}

void UDynamicTextureComponent::Fill(const FLinearColor& Color) const
{
	// Get the base pointer of the pixel buffer
	uint8* Ptr = PixelBuffer.Get();

	// Iterate over all pixels
	for (int i = 0; i < TextureWidth * TextureHeight; ++i)
	{
		SetPixelInternal(Ptr, Color.R * 255, Color.G * 255, Color.B * 255, Color.A * 255);

		// Advance to the next pixel
		Ptr += DYNAMIC_TEXTURE_BYTES_PER_PIXEL;
	}

	// UpdateTexture();
}

void UDynamicTextureComponent::SetAllPixels(const TArray<FLinearColorArray>& Colors) const
{
	// return if there is no enough information of colors
	if(!IsValidColorArray(Colors))
	{
		return;
	}

	// Get the base pointer of the pixel buffer
	uint8* Ptr = PixelBuffer.Get();

	// Iterate over all pixels
	for (int Y = 0; Y < TextureHeight; ++Y)
	{
		for (int X = 0; X < TextureWidth; ++X)
		{
			const FLinearColor Color = Colors[Y][X];

			// UE_LOG(LogTemp, Warning, TEXT("[Y, X]- [%s, %s] || FlinearColor- %s."), *FString::FromInt(Y), *FString::FromInt(X), *Color.ToString());
			// Set the pixel
			SetPixelInternal(Ptr, Color.R * 255, Color.G * 255, Color.B * 255, Color.A * 255);

			// Advance to the next pixel
			Ptr += DYNAMIC_TEXTURE_BYTES_PER_PIXEL;
		}	
	}

	// UpdateTexture();
}

void UDynamicTextureComponent::FillRect(const int32& X, const int32& Y, const int32& Width, const int32& Height, const FLinearColor& Color) const
{
	// Will hold the current pixel
	uint8* Ptr = nullptr;

	// Loop over the Y and X region
	for (int y = Y; y < Y + Height; y++)
	{
		for (int x = X; x < X + Width; x++)
		{
			// Get the current pixel pointer
			Ptr = GetPointerToPixel(x, y);

			// Set the pixel
			SetPixelInternal(Ptr, Color.R * 255, Color.G * 255, Color.B * 255, Color.A * 255);
		}
	}

	// UpdateTexture();
}

void UDynamicTextureComponent::Clear() const
{
	// Fill with the clear color
	Fill(ClearColor);
}

UTexture2D* UDynamicTextureComponent::GetTextureResource() const
{
	return Texture;
}

void UDynamicTextureComponent::SetPixelInternal(uint8*& Ptr, const uint8& Red, const uint8& Green, const uint8& Blue, const uint8& Alpha)
{
	// Pixels are stored in BGRA format, Don't change the order
	*Ptr = Blue;
	*(Ptr + 1) = Green;
	*(Ptr + 2) = Red;
	*(Ptr + 3) = Alpha;
}

uint8* UDynamicTextureComponent::GetPointerToPixel(const int32& X, const int32& Y) const
{
	// The calculation of the pointer address of a given pixel is
	// base + ((x + (y * width)) * bpp)
	return (PixelBuffer.Get() + ((X + (Y * TextureWidth)) * DYNAMIC_TEXTURE_BYTES_PER_PIXEL));
}

void UDynamicTextureComponent::UpdateTexture() const
{
	if(bIsInitializing)
	{
		return;
	}
	
	// Make sure the proxy and the texture is valid
	if (UpdateTextureRegionProxy.IsValid() && Texture)
	{
		// Update the texture's regions
		Texture->UpdateTextureRegions(
			0,											// Mip index
			1,											// Number of regions
			UpdateTextureRegionProxy.Get(),				// Region proxy
			TextureWidth * DYNAMIC_TEXTURE_BYTES_PER_PIXEL,	// Source data pitch
			DYNAMIC_TEXTURE_BYTES_PER_PIXEL,			// Bytes per pixel of source data
			PixelBuffer.Get()							// Buffer of pixels to set
		);
	}
}

int32 UDynamicTextureComponent::GetWidth() const
{
	return TextureWidth;
}

int32 UDynamicTextureComponent::GetHeight() const
{
	return TextureHeight;
}

bool UDynamicTextureComponent::IsValidColorArray(const TArray<FLinearColorArray>& Colors) const
{
	const int32 ArrayHeight = Colors.Num();
	if(ArrayHeight <= 0)
	{
		return false;
	}
	const int32 ArrayWidth = Colors[0].Num();
	
	return((ArrayWidth == TextureWidth) && (ArrayHeight == TextureHeight));
}

void UDynamicTextureComponent::InitializePixelColorsInternal(const TArray<FLinearColorArray>& PixelColors) const
{
	if(IsValidColorArray(PixelColors))
	{
		SetAllPixels(PixelColors);
	}
	else
	{
		Clear();
	}
}


TObjectPtr<UTexture2D> UDynamicTextureComponent::CreateTextureInternal(const int32& InWidth, const int32& InHeight,
                                                                       const TextureFilter& FilterMethod) const
{
	const TObjectPtr<UTexture2D> OutTexture = UTexture2D::CreateTransient(InWidth, InHeight);
	OutTexture->MipGenSettings = TextureMipGenSettings::TMGS_NoMipmaps;
	OutTexture->CompressionSettings = TextureCompressionSettings::TC_VectorDisplacementmap; // The VectorDisplacementMap is a raw RGBA8 format
	OutTexture->SRGB = 1;
	OutTexture->Filter = FilterMethod;
	OutTexture->UpdateResource();

	return OutTexture;
}

bool UDynamicTextureComponent::ShouldUpdateTextureSize(const int32& InWidth, const int32& InHeight) const
{
	// Dont run if texture dimensions are not valid.
	if(!ValidTextureSize(InWidth, InHeight))
	{
		return false;
	}

	if((InWidth == TextureWidth) && (InHeight == TextureHeight))
	{
		return false;
	}
	return true;
}

bool UDynamicTextureComponent::ValidTextureSize(const int32& InWidth, const int32& InHeight)
{
	if((InWidth <= 0) || (InHeight <= 0))
	{
		return false;
	}

	return true;
}

void UDynamicTextureComponent::MakeTextureRegionProxy(const int32& InWidth, const int32& InHeight, const bool bForceMake)
{
	if(bForceMake)
	{
		UpdateTextureRegionProxy.Reset();
		UpdateTextureRegionProxy = MakeUnique<FUpdateTextureRegion2D>(0, 0, 0, 0, InWidth, InHeight);
		return;
	}
	if(!UpdateTextureRegionProxy.IsValid())
	{
		UpdateTextureRegionProxy = MakeUnique<FUpdateTextureRegion2D>(0, 0, 0, 0, InWidth, InHeight);
		return;
	}
	UpdateTextureRegionProxy.Get()->Width = InWidth;
	UpdateTextureRegionProxy.Get()->Height = InHeight;
}

void UDynamicTextureComponent::MakePixelBuffer(const int32& InWidth, const int32& InHeight)
{
	if(PixelBuffer.IsValid())
	{
		// Reset it to make sure there are no conflicts
		PixelBuffer.Reset();
	}
	const SIZE_T BufferSize = InWidth * InHeight * DYNAMIC_TEXTURE_BYTES_PER_PIXEL;
	PixelBuffer = MakeUnique<uint8[]>(BufferSize);
}
