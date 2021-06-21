#include "GridPathfinders/GridPathfinder.h"
#include "Subsystems/GridSubsystem.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

FGridPathfindingRequest::FGridPathfindingRequest()
{
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

bool UGridPathfinder::IsReachable_Implementation(UGrid* Start, UGrid* Dest)
{
	if (UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(GetWorld()))
	{
		UNavigationPath* Path = NavigationSystem->FindPathToLocationSynchronously(GetWorld(),Start->GetCenter(),Dest->GetCenter());
		bool bResult = !Dest->IsEmpty() && Path && Path->IsValid() && !Path->IsPartial();
		return bResult;
	}
	return false;
}

int32 UGridPathfinder::Heuristic_Implementation(UGrid* From, UGrid* To)
{
	return GetCost(From, To);
}
