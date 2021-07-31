#include "GridPathfinders/AStar.h"

#include "GridPathfinders/GridPathfinder.h"
#include "Grids/Grid.h"

FAStar::FAStar(UGrid* _Start, UGrid* _Goal, UGridPathfinder* _PathFinder, bool _Reversed)
		: bSuccess(false), Reversed(_Reversed), Start(_Start), Goal(_Goal), PathFinder(_PathFinder)
{
	Comparer.FCost = &FCost;

	GCost.Add(Start, 0);

	FCost.Add(Start, PathFinder->GetCost(Start, Goal));

	OpenSet.HeapPush(Start, Comparer);
}

bool FAStar::Step()
{
	if (OpenSet.Num() == 0)
		return false;

	UGrid* Current = nullptr;
	OpenSet.HeapPop(Current, Comparer);

	if (Current->Equal(Goal))
	{
		bSuccess = true;
		return true;
	}

	CloseSet.Add(Current);

	TArray<UGrid*> Neighbors;
	Current->GetNeighbors(Neighbors);

	for (int i = 0; i < Neighbors.Num(); ++i)
	{
		UGrid* Next = Neighbors[i];

		if (CloseSet.Contains(Next) || !(Reversed ? PathFinder->IsReachable(Next, Current) : PathFinder->IsReachable(Current, Next)))
			continue;

		int NewCost = GCost[Current] + (Reversed ? PathFinder->GetCost(Next, Current) : PathFinder->GetCost(Current, Next));
		if (!GCost.Contains(Next) || NewCost < GCost[Next])
		{
			GCost.Add(Next, NewCost);

			FCost.Add(Next, NewCost + (Reversed ? PathFinder->Heuristic(Goal, Next) : PathFinder->Heuristic(Next, Goal)));

			if (!OpenSet.Contains(Next))
				OpenSet.HeapPush(Next, Comparer);

			CameFrom.Add(Next, Current);
		}
	}
	return true;
}

UGrid* FAStar::GetIntersect(const FAStar& R)
{
	for (UGrid* Grid : CloseSet)
	{
		if (R.CloseSet.Contains(Grid))
			return Grid;
	}

	for (UGrid* Grid : R.CloseSet)
	{
		if (CloseSet.Contains(Grid))
			return Grid;
	}
	return nullptr;
}

void FAStar::CollectPath(UGrid* StartGrid, TArray<UGrid*>& Result)
{
	UGrid* CurrentGrid = StartGrid;
	Result.AddUnique(CurrentGrid);

	while (CurrentGrid != Start)
	{
		CurrentGrid = CameFrom.FindChecked(CurrentGrid);
		Result.AddUnique(CurrentGrid);
	}
}

FBidirectionalAStar::FBidirectionalAStar(UGrid* _Start, UGrid* _Goal, UGridPathfinder* _PathFinder)
		:bSuccess(false), IntersectGrid(nullptr), ForwardAStar(_Start, _Goal, _PathFinder), BackwardAStar(_Goal, _Start, _PathFinder, true)
{}

bool FBidirectionalAStar::Step()
{
	if (!ForwardAStar.Step())
		return false;

	if (ForwardAStar.bSuccess)
	{
		IntersectGrid = ForwardAStar.Goal;
	}
	else
	{
		if (!BackwardAStar.Step())
			return false;

		if (BackwardAStar.bSuccess)
			IntersectGrid = BackwardAStar.Start;
	}

	if (IntersectGrid == nullptr)
		IntersectGrid = ForwardAStar.GetIntersect(BackwardAStar);

	if (IntersectGrid != nullptr)
	{
		bSuccess = true;
	}
	return true;
}

void FBidirectionalAStar::CollectPath(TArray<UGrid*>& Result)
{
	ForwardAStar.CollectPath(IntersectGrid, Result);
	Algo::Reverse(Result);
	BackwardAStar.CollectPath(IntersectGrid, Result);
}
