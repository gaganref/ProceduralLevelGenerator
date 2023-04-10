// // Fill out your copyright notice in the Description page of Project Settings.
//
// #pragma once
//
// #include "CoreMinimal.h"
// #include "UObject/Object.h"
// #include "QuadTree.generated.h"
//
//
// USTRUCT()
// struct FQuadTreeNode
// {
// 	GENERATED_BODY()
//
// 	UPROPERTY()
// 	FVector2D Center;
//
// 	UPROPERTY()
// 	FVector2D Size;
// 	
// 	UPROPERTY()
// 	TArray<FQuadTreeNode> Children;
//
// 	FQuadTreeNode() {}
// 	
// 	FQuadTreeNode(const FVector2D& InCenter, const FVector2D& InSize, const uint8& InDepth/**, const FBox2D& InBounds*/)
// 		: Center(InCenter), Size(InSize)/**, Bounds(InBounds)*/ {}
//
// 	friend FArchive& operator<<(FArchive& Ar, FQuadTreeNode& Node);
// };
//
// /**
//  * 
//  */
//
// UCLASS(Blueprintable)
// class PROCEDURALLEVELGENERATOR_API UQuadTree : public UObject
// {
// 	GENERATED_BODY()
//
// private:
//
// 	UPROPERTY()
// 	FQuadTreeNode RootNode;
//
// 	UPROPERTY()
// 	int32 MaxDepth;
//
// public:
// 	
// 	UFUNCTION(BlueprintCallable, Category = "Quadtree")
// 	void Initialize(const FVector2D& InCenter, const FVector2D& InSize, const int32& InMaxDepth);
//
// 	UFUNCTION(BlueprintCallable, Category = "Quadtree")
// 	TArray<FVector2D> GetPointsWithinRadius(const FVector2D& Center, float Radius) const;
//
// 	UFUNCTION(BlueprintCallable, Category = "Quadtree")
// 	FVector2D GetGridSize() const;
//
// 	UFUNCTION(BlueprintCallable, Category = "Quadtree")
// 	void InsertPoint(const FVector2D& Point);
// 	
// 	UFUNCTION(BlueprintCallable, Category = "Quadtree")
// 	void SerializeToByteArray(TArray<uint8>& OutData);
//
// 	UFUNCTION(BlueprintCallable, Category = "Quadtree")
// 	void DeserializeFromByteArray(const TArray<uint8>& InData);
// 	
// private:
//
// 	void SubdivideNode(FQuadTreeNode& Node, const int32& CurrentDepth, const int32& MaxDepth);
//
// 	TArray<FVector2D> GetPointsWithinRadiusRecursive(const FVector2D& Center, float Radius, const FQuadTreeNode& Node) const;
//
// 	void InsertPointRecursive(const FVector2D& Point, FQuadTreeNode& Node, int32 CurrentDepth, int32 MaxDepth);
//
// 	int32 GetQuadrant(const FVector2D& Point, const FVector2D& Center) const;
// };
//
//
