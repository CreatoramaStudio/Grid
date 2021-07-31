#include "GridPathfinders/GridPathfinder.h"

#include "LogGridRuntime.h"
#include "Subsystems/GridSubsystem.h"
#include "NavigationSystem.h"

#include "NavigationPath.h"
#include "GridPathfinders/AStar.h"

FGridPathfindingRequest::FGridPathfindingRequest()
{
	Sender = nullptr;
	bRemoveDest = false;
	MaxCost = -1;
	MaxSearchStep = 1000;
	Start = Destination = nullptr;
}

UGridPathfinder::UGridPathfinder()
{
	GridSubsystem = nullptr;
}

UGridPathfinder::~UGridPathfinder()
{
}

UGrid* UGridPathfinder::GetStart() const
{
	return Request.Start;
}

UGrid* UGridPathfinder::GetDestination() const
{
	return Request.Destination;
}

AActor* UGridPathfinder::GetSender() const
{
	return Request.Sender;
}

UGridSubsystem* UGridPathfinder::GetGridSubsystem() const
{
	return GridSubsystem;
}

const FGameplayTagContainer& UGridPathfinder::GetExtraTags() const
{
	return Request.ExtraTags;
}

bool UGridPathfinder::FindPath(const FGridPathfindingRequest& request, TArray<UGrid*>& Result)
{
	Result.Reset();

	bool bSuccess = false;

	Request = request;

	UGrid* Start = Request.Start;
	UGrid* Goal = Request.Destination;

	FBidirectionalAStar BidirectionalAStar(Start, Goal, this);

	int32 Step = 0;
	while (!bSuccess)
	{
		if (!BidirectionalAStar.Step())
		{
			break;
		}

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
				Cost += GetCost(Result[i - 1], Result[i]);

				if (Cost > Request.MaxCost)
				{
					break;
				}
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

bool UGridPathfinder::GetReachableGrids(AActor* Sender, const int32 MaxCost, TArray<UGrid*>& Result)
{
	Result.Reset();

	if (!Sender || MaxCost < 0)
	{
		return false;
	}

	Request.Sender = Sender;
	Request.MaxCost = MaxCost;

	TQueue<UGrid*> OpenSet;
	TSet<UGrid*> CloseSet;
	TMap<UGrid*, int32> Cost;
	UGrid* Start = GetGridSubsystem()->GetGridByPosition(Sender->GetActorLocation());

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

			Request.Destination = Next;

			if (CloseSet.Contains(Next) || !IsReachable(Current, Next))
			{
				continue;
			}

			int NewCost = Cost[Current] + GetCost(Current, Next);

			if (NewCost > MaxCost)
			{
				continue;
			}

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

bool UGridPathfinder::IsReachable_Implementation(UGrid* Start, UGrid* Dest)
{
	if (UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		UNavigationPath* Path = NavigationSystem->FindPathToLocationSynchronously(GetWorld(),Start->GetCenter(),Dest->GetCenter());
		const bool bResult = !Dest->IsEmpty() && Path && Path->IsValid() && !Path->IsPartial();
		return bResult;
	}
	
	FLogGridRuntime::Error("Navigation System not found");
	return false;
}

int32 UGridPathfinder::Heuristic_Implementation(UGrid* From, UGrid* To)
{
	return GetCost(From, To);
}
