// // Fill out your copyright notice in the Description page of Project Settings.
//
//
// #include "QuadTree/QuadTree.h"
//
// #include "GenericQuadTree.h"
// #include "Serialization/MemoryWriter.h"
// #include "Serialization/MemoryReader.h"
//
// #pragma region QuadTreeNode
//
// FArchive& operator<<(FArchive& Ar, FQuadTreeNode& Node)
// {
// 	Ar << Node.Center;
// 	Ar << Node.Size;
//
// 	if (Ar.IsLoading())
// 	{
// 		int32 ChildrenCount;
// 		Ar << ChildrenCount;
// 		Node.Children.SetNum(ChildrenCount);
// 	}
// 	else if (Ar.IsSaving())
// 	{
// 		int32 ChildrenCount = Node.Children.Num();
// 		Ar << ChildrenCount;
// 	}
//
// 	for (FQuadTreeNode& Child : Node.Children)
// 	{
// 		Ar << Child;
// 	}
//
// 	return Ar;
// }
//
// #pragma endregion
//
//
// void UQuadTree::Initialize(const FVector2D& InCenter, const FVector2D& InSize, const int32& InMaxDepth)
// {
// 	RootNode.Center = FVector2D(InSize.X / 2, InSize.Y / 2);
// 	RootNode.Size = InSize;
// 	MaxDepth = InMaxDepth;
// }
//
// TArray<FVector2D> UQuadTree::GetPointsWithinRadius(const FVector2D& Center, float Radius) const
// {
// 	return GetPointsWithinRadiusRecursive(Center, Radius, RootNode);
// }
//
// void UQuadTree::SubdivideNode(FQuadTreeNode& Node, const int32& CurrentDepth, const int32& MaxDepth)
// {
// 	if (CurrentDepth >= MaxDepth)
// 	{
// 		return;
// 	}
//
// 	const FVector2D HalfSize = Node.Size / 2.0f;
// 	Node.Children.Reserve(4);
//
// 	for (int32 Y = -1; Y <= 1; Y += 2)
// 	{
// 		for (int32 X = -1; X <= 1; X += 2)
// 		{
// 			FVector2D ChildCenter = Node.Center + FVector2D(X * HalfSize.X / 2.0f, Y * HalfSize.Y / 2.0f);
// 			FQuadTreeNode ChildNode(ChildCenter, HalfSize, CurrentDepth + 1);
// 			SubdivideNode(ChildNode, CurrentDepth + 1, MaxDepth);
// 			Node.Children.Add(ChildNode);
// 		}
// 	}
// }
//
// TArray<FVector2D> UQuadTree::GetPointsWithinRadiusRecursive(const FVector2D& Center, float Radius,
// 	const FQuadTreeNode& Node) const
// {
// 	TArray<FVector2D> Points;
//
// 	FVector2D Min = Node.Center - Node.Size / 2.0f;
// 	FVector2D Max = Node.Center + Node.Size / 2.0f;
//
// 	FBox2D Bounds(Min - FVector2D(Radius, Radius), Max + FVector2D(Radius, Radius));
// 	
// 	if (!Bounds.IsInside(Center))
// 	{
// 		return Points;
// 	}
//
// 	if (Node.Children.Num() == 0)
// 	{
// 		if ((Node.Center - Center).Size() <= Radius)
// 		{
// 			Points.Add(Node.Center);
// 		}
// 		return Points;
// 	}
//
// 	for (const FQuadTreeNode& Child : Node.Children)
// 	{
// 		Points.Append(GetPointsWithinRadiusRecursive(Center, Radius, Child));
// 	}
//
// 	return Points;
// }
//
// FVector2D UQuadTree::GetGridSize() const
// {
// 	return FVector2D(RootNode.Size.X, RootNode.Size.Y);
// }
//
// void UQuadTree::InsertPoint(const FVector2D& Point)
// {
// 	// const int32 InitialDepth = 0;
// 	InsertPointRecursive(Point, RootNode, 0, MaxDepth);
// }
//
// void UQuadTree::InsertPointRecursive(const FVector2D& Point, FQuadTreeNode& Node, int32 CurrentDepth, int32 MaxDepth)
// {
// 	if (Node.Children.Num() == 0)
// 	{
// 		Node.Center = Point;
// 		if (CurrentDepth < MaxDepth)
// 		{
// 			Node.Children.SetNum(4);
// 			SubdivideNode(Node, CurrentDepth, MaxDepth);
// 		}
// 		return;
// 	}
//
// 	const int32 Quadrant = GetQuadrant(Point, Node.Center);
// 	InsertPointRecursive(Point, Node.Children[Quadrant], CurrentDepth + 1, MaxDepth);
// }
//
// int32 UQuadTree::GetQuadrant(const FVector2D& Point, const FVector2D& Center) const
// {
// 	if (Point.X >= Center.X)
// 	{
// 		if (Point.Y >= Center.Y)
// 		{
// 			return 0; // Top-right quadrant
// 		}
// 		
// 		return 3; // Bottom-right quadrant
// 	}
// 	
// 	if (Point.Y >= Center.Y)
// 	{
// 		return 1; // Top-left quadrant
// 	}
// 	
// 	return 2; // Bottom-left quadrant
// }
//
// void UQuadTree::SerializeToByteArray(TArray<uint8>& OutData)
// {
// 	FMemoryWriter MemoryWriter(OutData);
// 	MemoryWriter << RootNode;
// }
//
// void UQuadTree::DeserializeFromByteArray(const TArray<uint8>& InData)
// {
// 	FMemoryReader MemoryReader(InData);
// 	MemoryReader << RootNode;
// }
