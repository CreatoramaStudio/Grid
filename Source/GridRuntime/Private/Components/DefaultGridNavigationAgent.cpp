#include "Components/DefaultGridNavigationAgent.h"
#include "GridRuntimeLog.h"
#include "NavigationSystem.h"
#include "NavigationPath.h"

UDefaultGridNavigationAgent::UDefaultGridNavigationAgent()
{
	AcceptanceRadius = 10.f;
}

UDefaultGridNavigationAgent::~UDefaultGridNavigationAgent()
{

}

bool UDefaultGridNavigationAgent::Check_Implementation(APawn* Pawn, UGrid* From, UGrid* To)
{
	if (Pawn == nullptr || From == nullptr || To == nullptr || Cast<AAIController>(Pawn->GetController()) == nullptr)
		return false;

	UNavigationSystemV1* NavSys = UNavigationSystemV1::GetCurrent(Pawn->GetWorld());
	UNavigationPath* Path = NavSys->FindPathToLocationSynchronously(Pawn->GetWorld(), From->GetCenter(), To->GetCenter(), Pawn);

	if (Path == nullptr || !Path->IsValid() || Path->IsPartial())
		return false;

	return true;
}

bool UDefaultGridNavigationAgent::RequestMove_Implementation(APawn* Pawn, UGrid* From, UGrid* To)
{
	if (Pawn == nullptr || From == nullptr || To == nullptr)
		return false;

	CurrentPawn = Pawn;
	CurrentController = Cast<AAIController>(Pawn->GetController());

	if (CurrentController == nullptr)
		return false;

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
			PrintErrorGridRuntime("UDefaultGridNavigationAgent::RequestMove_Implementation failed, Destination is " + Dest.ToString());
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
			PrintErrorGridRuntime("UDefaultGridNavigationAgent::OnAIControllerMoveCompleted failed, Result: " +  Result);
		}
		break;
	}

	APawn* TempPawn = CurrentPawn;

	CurrentController->ReceiveMoveCompleted.RemoveAll(this);
	CurrentPawn = nullptr;
	CurrentController = nullptr;

	OnMoveCompleted.Broadcast(TempPawn, bSuccess);
}
