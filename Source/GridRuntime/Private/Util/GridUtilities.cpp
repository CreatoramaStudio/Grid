#include "Util/GridUtilities.h"
#include "LogGridRuntime.h"
#include "Subsystems/GridSubsystem.h"
#include "Containers/Queue.h"

class FAStar
{
public:
	FAStar(UGrid* _Start, UGrid* _Goal, UGridPathfinder* _PathFinder, bool _Reversed = false)
		: bSuccess(false), Reversed(_Reversed), Start(_Start), Goal(_Goal), PathFinder(_PathFinder)
	{
		Comparer.FCost = &FCost;

		GCost.Add(Start, 0);

		FCost.Add(Start, PathFinder->GetCost(Start, Goal));

		OpenSet.HeapPush(Start, Comparer);
	}

	bool Step()
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

	UGrid* GetIntersect(const FAStar& R)
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

	void CollectPath(UGrid* StartGrid, TArray<UGrid*>& Result)
	{
		UGrid* CurrentGrid = StartGrid;
		Result.AddUnique(CurrentGrid);

		while (CurrentGrid != Start)
		{
			CurrentGrid = CameFrom.FindChecked(CurrentGrid);
			Result.AddUnique(CurrentGrid);
		}
	}

	bool bSuccess;
private:
	struct FComparer
	{
		bool operator()(const UGrid& L, const UGrid& R) const
		{
			const int32 LFCost = FCost->Contains(&L) ? FCost->FindChecked(&L) : TNumericLimits<int32>::Max();
			const int32 RFCost = FCost->Contains(&R) ? FCost->FindChecked(&R) : TNumericLimits<int32>::Max();
			return LFCost < RFCost;
		}
		TMap<UGrid*, int32>* FCost;
	} Comparer;

	bool Reversed;
	TArray<UGrid*> OpenSet;
	TSet<UGrid*> CloseSet;
	TMap<UGrid*, int32> FCost;
	TMap<UGrid*, int32> GCost;
	TMap<UGrid*, UGrid*> CameFrom;
	UGrid* Start;
	UGrid* Goal;
	UGridPathfinder* PathFinder;

	friend class FBidirectionalAStar;
};

class FBidirectionalAStar
{
public:
	FBidirectionalAStar(UGrid* _Start, UGrid* _Goal, UGridPathfinder* _PathFinder)
		:bSuccess(false), IntersectGrid(nullptr), ForwardAStar(_Start, _Goal, _PathFinder), BackwardAStar(_Goal, _Start, _PathFinder, true)
	{}

	bool Step()
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

	void CollectPath(TArray<UGrid*>& Result)
	{
		ForwardAStar.CollectPath(IntersectGrid, Result);
		Algo::Reverse(Result);
		BackwardAStar.CollectPath(IntersectGrid, Result);
	}

	bool bSuccess;
private:
	UGrid * IntersectGrid;
	FAStar ForwardAStar;
	FAStar BackwardAStar;
};

uint64 UGridUtilities::GetUniqueIdByCoordinate(const FIntVector& Coord)
{
	return ((uint64)Coord.Z << 44) + ((uint64)Coord.Y << 22) + (uint64)Coord.X;
}

FVector UGridUtilities::CalcGridDecalSize(const EGridType GridType, const float GridSize)
{
	switch (GridType)
	{
	case EGridType::Square:
		{
			FVector DecalSize(GridSize, GridSize, GridSize);

			DecalSize /= FVector(4.f, 2.f, 2.f);

			return DecalSize;
		}
	case EGridType::Hexagon:
		{
			FVector DecalSize(GridSize, GridSize * 2, GridSize * FMath::Sqrt(3));

			DecalSize /= 2;

			return DecalSize;
		}
	default:
		FLogGridRuntime::Error("UGridUtilities::CalcGridDecalSize unknown grid type");
		return FVector::ZeroVector;
	}
}

bool UGridUtilities::FindPath(const FGridPathfindingRequest& Request, UGridPathfinder* PathFinder, TArray<UGrid*>& Result)
{
	Result.Reset();

	bool bSuccess = false;

	PathFinder->Request = Request;

	UGrid* Start = Request.Start;
	UGrid* Goal = Request.Destination;

	FBidirectionalAStar BidirectionalAStar(Start, Goal, PathFinder);

	int32 Step = 0;
	while (!bSuccess)
	{
		if (!BidirectionalAStar.Step())
			break;

		bSuccess = BidirectionalAStar.bSuccess;

		if (++Step > Request.MaxSearchStep)
		{
			FLogGridRuntime::Warning("AGridSubsystem::FindPath failed, out of MaxFindStep");
			break;
		}
	}

	if (bSuccess)
	{
		BidirectionalAStar.CollectPath(Result);

		if (Request.bRemoveDest)
		{
			Result.Pop();
		}

		if (Request.MaxCost >= 0)
		{
			int32 Cost = 0;
			int32 i;
			for (i = 1; i < Result.Num(); ++i)
			{
				Cost += PathFinder->GetCost(Result[i - 1], Result[i]);

				if (Cost > Request.MaxCost)
					break;
			}

			if (i < Result.Num())
			{
				Result.RemoveAt(i, Result.Num() - i);
				bSuccess = false;
			}
		}
	}
	return bSuccess;
}

bool UGridUtilities::GetReachableGrids(AActor* Sender, int32 MaxCost, UGridPathfinder* PathFinder, TArray<UGrid*>& Result)
{
	Result.Reset();

	if (Sender == nullptr || MaxCost < 0)
		return false;

	PathFinder->Request.Sender = Sender;
	PathFinder->Request.MaxCost = MaxCost;

	TQueue<UGrid*> OpenSet;
	TSet<UGrid*> CloseSet;
	TMap<UGrid*, int32> Cost;
	UGrid* Start = PathFinder->GetGridSubsystem()->GetGridByPosition(Sender->GetActorLocation());

	OpenSet.Enqueue(Start);
	Result.Add(Start);

	Cost.Add(Start, 0);

	while (!OpenSet.IsEmpty())
	{
		UGrid* Current;
		OpenSet.Dequeue(Current);

		CloseSet.Add(Current);

		TArray<UGrid*> Neighbors;
		Current->GetNeighbors(Neighbors);

		for (int i = 0; i < Neighbors.Num(); ++i)
		{
			UGrid* Next = Neighbors[i];

			PathFinder->Request.Destination = Next;

			if (CloseSet.Contains(Next) || !PathFinder->IsReachable(Current, Next))
				continue;

			int NewCost = Cost[Current] + PathFinder->GetCost(Current, Next);

			if (NewCost > MaxCost)
				continue;

			Result.AddUnique(Next);

			if (!Cost.Contains(Next) || NewCost < Cost[Next])
			{
				Cost.Add(Next, NewCost);

				OpenSet.Enqueue(Next);
			}
		}
	}

	return Result.Num() > 0;
}

FIntVector UGridUtilities::Subtract_IntVectorIntVector(const FIntVector& L, const FIntVector& R)
{
	return L - R;
}

FIntVector UGridUtilities::Add_IntVectorIntVector(const FIntVector& L, const FIntVector& R)
{
	return L + R;
}

FIntVector UGridUtilities::Multiply_IntVectorInt(const FIntVector& L, int R)
{
	return L * R;
}

bool UGridUtilities::EqualEqual_GridGrid(const UGrid* A, const UGrid* B)
{
	return A->Equal(B);
}

bool UGridUtilities::NotEqual_GridGrid(const UGrid* A, const UGrid* B)
{
	return !A->Equal(B);
}

bool UGridUtilities::GridTraceSingleForObjects(const UGrid* Grid, const float TraceDistance, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, const bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, const EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, const bool bIgnoreSelf, const FLinearColor TraceColor, const FLinearColor TraceHitColor, const float DrawTime)
{
	const FVector Start = Grid->GetCenter() + FVector(0.f, 0.f, TraceDistance / 2.f);
	const FVector End = Grid->GetCenter() - FVector(0.f, 0.f, TraceDistance / 2.f);

	return UKismetSystemLibrary::LineTraceSingleForObjects(Grid->GridSubsystem->GetWorld(), Start, End, ObjectTypes
		, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit
		, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
}

void UGridUtilities::GetBoundsByGridArray(const TArray<UGrid*>& Grids, FBoxSphereBounds& Bounds)
{
	if (Grids.Num() == 0)
		return;

	Bounds = Grids[0]->Bounds;

	for (int i = 1; i < Grids.Num(); ++i)
	{
		Bounds = Bounds + Grids[i]->Bounds;
	}
}
