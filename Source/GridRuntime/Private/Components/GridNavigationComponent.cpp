#include "Components/GridNavigationComponent.h"
#include "GridRuntimeLog.h"
#include "Grid.h"
#include "GridManager.h"
#include "Components/DefaultGridNavigationAgent.h"
#include "Util/GridUtilities.h"

UGridNavigationComponent::UGridNavigationComponent()
{
	PrimaryComponentTick.bCanEverTick = true;

	OwnerPawn = nullptr;
	OwnerController = nullptr;

	FollowingPathIndex = 0;
	bIsMoving = false;

	NavMode = EGridNavMode::GridBased;

	AgentClasses.Add(UDefaultGridNavigationAgent::StaticClass());
}

UGridNavigationComponent::~UGridNavigationComponent()
{

}

void UGridNavigationComponent::BeginPlay()
{
	Super::BeginPlay();

	for (int i = 0; i < AgentClasses.Num(); ++i)
	{
		UGridNavigationAgent* Agent = NewObject<UGridNavigationAgent>(this, AgentClasses[i]);

		if (Agent != nullptr)
		{
			FScriptDelegate Delegate;
			Delegate.BindUFunction(this, "OnMoveCompleted");

			Agent->OnMoveCompleted.Add(Delegate);
			Agents.Add(Agent);
		}
		else
		{
			PrintErrorGridRuntime("UGridNavigationComponent::BeginPlay create grid navigation agent failed!");
		}
	}

	OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn != nullptr)
	{
		OwnerController = Cast<AAIController>(OwnerPawn->GetController());
	}
}

bool UGridNavigationComponent::RequestMove(UGrid* DestGrid)
{
	if (OwnerPawn == nullptr)
	{
		PrintErrorGridRuntime("UGridNavigationComponent::RequestMove failed, OwnerPawn is null");
		return false;
	}

	if (OwnerController == nullptr)
	{
		PrintErrorGridRuntime("UGridNavigationComponent::RequestMove failed, OwnerController is null");
		return false;
	}

	if (DestGrid == nullptr)
	{
		PrintWarningGridRuntime("UGridNavigationComponent::RequestMove failed, DestGrid is null");
		return false;
	}

	UGridManager* GridManager = DestGrid->GridManager;

	if (!ensure(GridManager != nullptr))
	{
		PrintErrorGridRuntime("UGridNavigationComponent::RequestMove failed, GridManager is null");
		return false;
	}

	FGridPathfindingRequest Request;
	TArray<UGrid*> Result;

	Request.Sender = OwnerPawn;
	Request.Destination = DestGrid;
	Request.Start = GridManager->GetGridByPosition(OwnerPawn->GetActorLocation());

	UGridPathFinder* PathFinder = GridManager->GetPathFinder();
	
	if (!PathFinder)
	{
		PrintErrorGridRuntime("UGridNavigationComponent::RequestMove failed, PathFinder is null");
		return false;
	}
	
	PathFinder->Reset();

	if (!UGridUtilities::FindPath(Request, PathFinder, CurrentFollowingPath))
	{
		return false;
	}

	FollowingPathIndex = 0;

	bIsMoving = true;

	MoveToNext();

	return true;
}

void UGridNavigationComponent::StopMove()
{
	if (CurrentAgent != nullptr)
	{
		CurrentAgent->StopMove();
		CurrentAgent = nullptr;
	}
}

bool UGridNavigationComponent::IsMoving() const
{
	return bIsMoving;
}

bool UGridNavigationComponent::MoveToNext()
{
	switch (NavMode)
	{
	case EGridNavMode::GridBased:
		return MoveToNextGrid();
	case EGridNavMode::Free:
		return MoveToNextPoint();
	default:
		return false;
	}
}

bool UGridNavigationComponent::MoveToNextGrid()
{
	++FollowingPathIndex;

	if (FollowingPathIndex >= CurrentFollowingPath.Num())
		return false;

	UGridManager* GridManager = CurrentFollowingPath.Last()->GridManager;

	UGrid* CurrGrid = GridManager->GetGridByPosition(OwnerPawn->GetActorLocation());
	UGrid* NextGrid = CurrentFollowingPath[FollowingPathIndex];

	CurrentAgent = FindAgent(CurrGrid, NextGrid);

	if (CurrentAgent == nullptr)
	{
		PrintErrorGridRuntime("UGridNavigationComponent::MoveToNextGrid can't find proper agent");
		return false;
	}

	CurrentAgent->RequestMove(OwnerPawn, CurrGrid, NextGrid);

	return true;
}

bool UGridNavigationComponent::MoveToNextPoint()
{
	++FollowingPathIndex;

	if (FollowingPathIndex >= CurrentFollowingPath.Num())
		return false;

	UGridManager* GridManager = CurrentFollowingPath.Last()->GridManager;

	UGrid* CurrGrid = GridManager->GetGridByPosition(OwnerPawn->GetActorLocation());
	UGrid* NextGrid = CurrentFollowingPath[FollowingPathIndex];

	UGridNavigationAgent* Agent = FindAgent(CurrGrid, NextGrid);
	
	if (Agent == nullptr)
	{
		PrintErrorGridRuntime("UGridNavigationComponent::MoveToNextGrid can't find proper agent");
		return false;
	}

	if (Cast<UDefaultGridNavigationAgent>(Agent) != nullptr)
	{
		int i;
		for (i = FollowingPathIndex; i < CurrentFollowingPath.Num() - 1; ++i)
		{
			if (!Agent->Check(OwnerPawn, CurrentFollowingPath[i], CurrentFollowingPath[i + 1]))
				break;
		}

		FollowingPathIndex = i;
		NextGrid = CurrentFollowingPath[FollowingPathIndex];
	}

	Agent->RequestMove(OwnerPawn, CurrGrid, NextGrid);

	return true;
}

UGridNavigationAgent* UGridNavigationComponent::FindAgent(UGrid* Start, UGrid* Goal)
{
	UGridNavigationAgent* Agent = nullptr;
	for (int i = 0; i < Agents.Num(); ++i)
	{
		if (Agents[i]->Check(OwnerPawn, Start, Goal))
		{
			Agent = Agents[i];
			break;
		}
	}
	return Agent;
}

void UGridNavigationComponent::OnMoveCompleted(APawn* Pawn, bool bSuccess)
{
	if (bSuccess)
	{
		if (FollowingPathIndex < CurrentFollowingPath.Num() - 1)
		{
			if (NavMode == EGridNavMode::GridBased)
				OnArrivalNewGrid.Broadcast(this);
		}
		else
		{
			OnArrivalGoal.Broadcast(this);
		}

		if (!MoveToNext())
		{
			bIsMoving = false;
		}
	}
	else
	{
		PrintErrorGridRuntime("UGridNavigationComponent::OnMoveCompleted failed");

		bIsMoving = false;
	}
}
