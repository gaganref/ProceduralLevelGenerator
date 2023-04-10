// // Fill out your copyright notice in the Description page of Project Settings.
//
//
// #include "QuadTree/QuadTreeHelpers.h"
//
// #include "QuadTree/QuadTree.h"
//
// bool UQuadTreeHelpers::InsertPointIntoQuadtree(FQuadTreeNode& Node, const FVector2D& Point, float MinDistance)
// {
// 	if (!Node.Bounds.IsInside(Point))
// 	{
// 		return false;
// 	}
//
// 	if (Node.Children.Num() > 0)
// 	{
// 		for (FQuadTreeNode& Child : Node.Children)
// 		{
// 			if (InsertPointIntoQuadtree(Child, Point, MinDistance))
// 			{
// 				return true;
// 			}
// 		}
//
// 		return false;
// 	}
// 	else
// 	{
// 		if (Node.Center.X < 0)
// 		{
// 			Node.Center = Point;
// 			return true;
// 		}
// 		else
// 		{
// 			if (FVector2D::Distance(Node.Center, Point) < MinDistance)
// 			{
// 				return false;
// 			}
//
// 			FVector2D HalfSize = Node.Bounds.GetSize() / 4;
// 			FVector2D Center = Node.Bounds.GetCenter();
// 			FBox2D Bounds;
//
// 			Bounds = FBox2D(Center - FVector2D(HalfSize.X * 3, HalfSize.Y * 3), Center - FVector2D(HalfSize.X, HalfSize.Y * 3));
// 			Node.Children.Add(new FQuadTreeNode(FVector2D(-1, -1), Bounds));
//
// 			Bounds = FBox2D(Center - FVector2D(HalfSize.X * 3, HalfSize.Y), Center - FVector2D(HalfSize.X, HalfSize.Y));
// 			Node.Children.Add(new FQuadTreeNode(FVector2D(-1, -1), Bounds));
//
// 			Bounds = FBox2D(Center - FVector2D(HalfSize.X, HalfSize.Y * 3), Center - FVector2D(0, HalfSize.Y * 3));
// 			Node.Children.Add(new FQuadTreeNode(FVector2D(-1, -1), Bounds));
//
// 			Bounds = FBox2D(Center - FVector2D(HalfSize.X, HalfSize.Y), Center);
// 			Node.Children.Add(new FQuadTreeNode(FVector2D(-1, -1), Bounds));
//
// 			for (int32 i = 0; i < 4; i++)
// 			{
// 				if (InsertPointIntoQuadtree(Node.Children[i], Node.Center, MinDistance))
// 				{
// 					break;
// 				}
// 			}
//
// 			Node.Center = FVector2D(-1, -1);
//
// 			for (int32 i = 0; i < 4; i++)
// 			{
// 				if (InsertPointIntoQuadtree(Node.Children[i], Point, MinDistance))
// 				{
// 					return true;
// 				}
// 			}
//
// 			return false;
// 		}
// 	}
// }
