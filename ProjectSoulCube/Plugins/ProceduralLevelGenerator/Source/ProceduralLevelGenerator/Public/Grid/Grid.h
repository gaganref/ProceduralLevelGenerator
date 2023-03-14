// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Grid.generated.h"


/**
 * Struct that holds each cell data of the grid
 */
USTRUCT(BlueprintType)
struct PROCEDURALLEVELGENERATOR_API FCellVertices
{
	GENERATED_BODY()

	UPROPERTY(Category = "Cell Vertices Data", BlueprintReadOnly)
	FVector TopLeft;

	UPROPERTY(Category = "Cell Vertices Data", BlueprintReadOnly)
	FVector TopRight;
	
	UPROPERTY(Category = "Cell Vertices Data", BlueprintReadOnly)
	FVector BottomLeft;

	UPROPERTY(Category = "Cell Vertices Data", BlueprintReadOnly)
	FVector BottomRight;

	FCellVertices() = default;

	FCellVertices(const FVector& InTopLeft, const FVector& InTopRight, const FVector& InBottomLeft, const FVector& InBottomRight)
		: TopLeft(InTopLeft), TopRight(InTopRight), BottomLeft(InBottomLeft), BottomRight(InBottomRight) {}
};


/**
 * Struct that holds each cell data of the grid
 */
USTRUCT(BlueprintType)
struct PROCEDURALLEVELGENERATOR_API FGridCell
{
	GENERATED_BODY()

	// Location is the center of the cell
	UPROPERTY(Category = "Cell Data", BlueprintReadOnly)
	FVector Location;

	UPROPERTY(Category = "Cell Vertices Data", BlueprintReadOnly)
	FCellVertices CellVertices;

	FGridCell() = default;

	explicit FGridCell(const FVector& InLocation) : Location(InLocation) {}

	// Constructor with cell vertices
	FGridCell(const FVector& InLocation, const FVector& InTopLeft, const FVector& InTopRight, const FVector& InBottomLeft, const FVector& InBottomRight)
		: Location(InLocation),	CellVertices(InTopLeft, InTopRight, InBottomLeft, InBottomRight){}
	
};



/**
 * 
 */
UCLASS(BlueprintType)
class PROCEDURALLEVELGENERATOR_API UGrid : public UDataAsset
{
	GENERATED_BODY()

protected:

	// Total no of rows(→) or total no of cells(■) in each row(→) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 Rows = 10;

	// Total no of columns(↑) or total no of cells(■) in each column(↑) in the grid.
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 Columns = 10;

	// Size of each individual cell (as a cell is a square SizeOnX = SizeOnY)
	UPROPERTY(Category = "Grid Data", EditAnywhere, BlueprintReadOnly, meta = (ClampMin = 10.0f))
	float CellSize = 100.0f;

	UPROPERTY()
	TArray<FGridCell> GridCells;

protected:
	
	/**
	 * Called after the C++ constructor and after the properties have been initialized, including those loaded from config.
	 * This is called before any serialization or other setup has happened.
	 */
	virtual void PostInitProperties() override;
	
	/**
	 * Called after properties are overwritten, including after subobjects initialization from a CDO.
	 * This could be called multiple times during an object lifetime, which is not the case for PostInitProperties which is expected to be called only once.
	 */
	virtual void PostReinitProperties() override;
	
#if WITH_EDITOR
	
	/**
	 * Called when a property on this object has been modified externally.
	 */
	virtual void PostEditChangeProperty( struct FPropertyChangedEvent& PropertyChangedEvent) override;

	/**
	 * This alternate version of PostEditChange is called when properties inside structs are modified.  The property that was actually modified
	 * is located at the tail of the list.  The head of the list of the FStructProperty member variable that contains the property that was modified.
	 */
	virtual void PostEditChangeChainProperty( struct FPropertyChangedChainEvent& PropertyChangedEvent ) override;

	/**
	 * Generic function that is used to validate objects during changelist validations, etc.
	 */
	virtual EDataValidationResult IsDataValid(TArray<FText>& ValidationErrors) override;
	
#endif

protected:

	/**
	 * Called to initialize grid.
	 */
	virtual void InitGrid();

public:

	// Getters
	
	FORCEINLINE int32 GetRows() const {return Rows;}
	FORCEINLINE int32 GetColumns() const {return Columns;}
	FORCEINLINE float GetCellSize() const {return CellSize;}
	
};
