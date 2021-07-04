#include "GridAgents/DefaultGridNavigationAgent.h"
#include "LogGridRuntime.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"
#include "Grids/Grid.h"

UDefaultGridNavigationAgent::UDefaultGridNavigationAgent()
{
	AcceptanceRadius = 10.f;
}

UDefaultGridNavigationAgent::~UDefaultGridNavigationAgent()
{

}

bool UDefaultGridNavigationAgent::Check_Implementation(APawn* Pawn, UGrid* From, UGrid* To)
{
	if (!Pawn || !From|| !To || !Cast<AAIController>(Pawn->GetController()))
	{
		return false;
	}

	UNavigationSystemV1* NavigationSystem = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
	UNavigationPath* Path = NavigationSystem->FindPathToLocationSynchronously(Pawn->GetWorld(), From->GetCenter(), To->GetCenter(), Pawn);

	return Path && Path->IsValid() && !Path->IsPartial();
}

bool UDefaultGridNavigationAgent::RequestMove_Implementation(APawn* Pawn, UGrid* From, UGrid* To)
{
	if (!Pawn || !From || !To)
	{
		return false;
	}

	CurrentPawn = Pawn;
	CurrentController = Cast<AAIController>(Pawn->GetController());
	
	if (!CurrentController)
	{
		return false;
	}

	FScriptDelegate Delegate;
	Delegate.BindUFunction(this, "OnAIControllerMoveCompleted");
	CurrentController->ReceiveMoveCompleted.AddUnique(Delegate);

	const EPathFollowingRequestResult::Type Result = CurrentController->MoveToLocation(To->GetCenter(), AcceptanceRadius, false);

	bool bSuccess = false;

	switch (Result)
	{
	case EPathFollowingRequestResult::Type::AlreadyAtGoal:
	case EPathFollowingRequestResult::Type::RequestSuccessful:
		{
			bSuccess = true;
			break;
		}
	case EPathFollowingRequestResult::Failed:
		{
			const FVector Dest = To->GetCenter();
			FLogGridRuntime::Error("UDefaultGridNavigationAgent::RequestMove_Implementation failed, Destination is " + Dest.ToString());
			break;
		}
	default:
		break;
	}

	return bSuccess;
}

void UDefaultGridNavigationAgent::StopMove_Implementation()
{
	if (CurrentController != nullptr)
	{
		CurrentController->ReceiveMoveCompleted.RemoveAll(this);
		CurrentController->StopMovement();

		CurrentPawn = nullptr;
		CurrentController = nullptr;
	}
}

void UDefaultGridNavigationAgent::OnAIControllerMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	bool bSuccess = false;

	switch (Result)
	{
	case EPathFollowingResult::Success:
		{
			bSuccess = true;
			break;
		}
	default:
		{
			FLogGridRuntime::Error("UDefaultGridNavigationAgent::OnAIControllerMoveCompleted failed, Result: " +  Result);
		}
		break;
	}

	APawn* TempPawn = CurrentPawn;

	CurrentController->ReceiveMoveCompleted.RemoveAll(this);
	CurrentPawn = nullptr;
	CurrentController = nullptr;

	OnMoveCompleted.Broadcast(TempPawn, bSuccess);
}
