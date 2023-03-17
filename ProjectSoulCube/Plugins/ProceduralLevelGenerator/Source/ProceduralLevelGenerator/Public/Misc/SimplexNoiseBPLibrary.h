/*

Ported from https://github.com/devdad/SimplexNoise with few changes
SimplexNoise 1.2.0
-----
DevDad - Afan Olovcic @ www.art-and-code.com - 08/12/2015

*/

#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "SimplexNoiseBPLibrary.generated.h"

UCLASS()
class PROCEDURALLEVELGENERATOR_API USimplexNoiseBPLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_UCLASS_BODY()
	
private:
	
	static unsigned char Perm[];
	
	static float  Grad(const int& Hash, const float& X);
	static float  Grad(const int& Hash, const float& X, const float& Y);
	static float  Grad(const int& Hash, const float& X, const float& Y, const float& Z);
	static float  Grad(const int& Hash, const float& X, const float& Y, const float& Z, const float& T);

public:
	
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static void SetNoiseSeed(const int32& NewSeed);
    
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static void SetNoiseFromStream(FRandomStream& RandStream);
	
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise1D_Raw(const float& X);
	
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise2D_Raw(const float& X, const float& Y);
	
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise3D_Raw(const float& X, const float& Y, const float& Z);
	
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise4D_Raw(const float& X, const float& Y, const float& Z, const float& W);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise1D(const float& X, const float& InFactor = 1.f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise2D(const float& X, const float& Y, const float& InFactor = 1.f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise3D(const float& X, const float& Y, const float& Z, const float& InFactor = 1.f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise4D(const float& X, const float& Y, const float& Z, const float& W, const float& InFactor = 1.f);

	// Scaled by float value
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseScaled1D(const float& X, const float& ScaleOut = 1.f, const float& InFactor = 1.f);
	
	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseScaled2D(const float& X, const float& Y, const float& ScaleOut = 1.f, const float& InFactor = 1.f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseScaled3D(const float& X, const float& Y, const float& Z, const float& ScaleOut = 1.f, const float& InFactor = 1.f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseScaled4D(const float& X, const float& Y, const float& Z, const float& W, const float& ScaleOut = 1.f, const float& InFactor =
		                                  1.f);

	// Return value in Range between two float numbers
	// Return Value is scaled by difference between rangeMin & rangeMax value

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseInRange1D(const float& X, const float& RangeMin, float RangeMax, const float& InFactor = 1.f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseInRange2D(const float& X, const float& Y, const float& RangeMin, float RangeMax, const float& InFactor = 1.f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseInRange3D(const float& X, const float& Y, const float& Z, const float& RangeMin, float RangeMax, const float& InFactor = 1.f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoiseInRange4D(const float& X, const float& Y, const float& Z, const float& W, const float& RangeMin, float RangeMax, const float&
		                                   InFactor = 1.f);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise1D_Ex(const float& X, const float& Lacunarity = 2.3f, const float& Persistence = 0.6f, const int& Octaves = 4, const float&
		                                  InFactor = 1.0f, const bool ZeroToOne = false);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise2D_Ex(const float& X, const float& Y, const float& Lacunarity = 2.3f, const float& Persistence = 0.6f, const int& Octaves = 4, const
	                                  float& InFactor = 1.0f, const bool ZeroToOne = false);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise3D_Ex(const float& X, const float& Y, const float& Z, const float& Lacunarity = 2.3f, const float& Persistence = 0.6f, const
	                                  int& Octaves = 4, const float& InFactor = 1.0f, const bool ZeroToOne = false);

	UFUNCTION(BlueprintCallable, Category = "SimplexNoise")
	static float SimplexNoise4D_Ex(const float& X, const float& Y, const float& Z, const float& W, const float& Lacunarity = 2.3f, const float&
		                                  Persistence = 0.6f, const int& Octaves = 4, const float& InFactor = 1.0f, const bool ZeroToOne = false);

};

