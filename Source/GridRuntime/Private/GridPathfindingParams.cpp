#include "GridPathfindingParams.h"
#include "GridManager.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

FGridPathfindingRequest::FGridPathfindingRequest()
{
	bRemoveDest = false;
	MaxCost = -1;
	MaxSearchStep = 1000;
	Start = Destination = nullptr;
}

UGridPathFinder::UGridPathFinder()
{
	GridManager = nullptr;
}

UGridPathFinder::~UGridPathFinder()
{
}

UGrid* UGridPathFinder::GetStart() const
{
	return Request.Start;
}

UGrid* UGridPathFinder::GetDestination() const
{
	return Request.Destination;
}

AActor* UGridPathFinder::GetSender() const
{
	return Request.Sender;
}

UGridManager* UGridPathFinder::GetGridManager() const
{
	return GridManager;
}

const FGameplayTagContainer& UGridPathFinder::GetExtraTags() const
{
	return Request.ExtraTags;
}

bool UGridPathFinder::IsReachable_Implementation(UGrid* Start, UGrid* Dest)
{
	if (UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		UNavigationPath* Path = NavigationSystem->FindPathToLocationSynchronously(GetWorld(),Start->GetCenter(),Dest->GetCenter());
		bool bResult = !Dest->IsEmpty() && Path && Path->IsValid() && !Path->IsPartial();
		return bResult;
	}
	return false;
}

int32 UGridPathFinder::Heuristic_Implementation(UGrid* From, UGrid* To)
{
	return GetCost(From, To);
}
