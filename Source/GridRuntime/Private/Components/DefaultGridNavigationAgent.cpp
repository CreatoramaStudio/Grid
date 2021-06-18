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
	UNavigationPath* Path = NavSys->FindPathToLocationSynchronously((UObject*)Pawn->GetWorld(), From->GetCenter(), To->GetCenter(), Pawn);

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
	Delegate.BindUFunction(this, "OnAIControllerMoveCompeleted");
	CurrentController->ReceiveMoveCompleted.AddUnique(Delegate);

	const EPathFollowingRequestResult::Type Result = CurrentController->MoveToLocation(To->GetCenter(), AcceptanceRadius, false);

	bool bSucc = false;

	switch (Result)
	{
	case EPathFollowingRequestResult::Type::AlreadyAtGoal:
	case EPathFollowingRequestResult::Type::RequestSuccessful:
		{
			bSucc = true;
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

	return bSucc;
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

void UDefaultGridNavigationAgent::OnAIControllerMoveCompeleted(FAIRequestID RequestID, EPathFollowingResult::Type Result)
{
	bool Succ = false;

	switch (Result)
	{
	case EPathFollowingResult::Success:
		{
			Succ = true;
			break;
		}
	default:
		{
			PrintErrorGridRuntime("UDefaultGridNavigationAgent::OnAIControllerMoveCompeleted failed, Result: " +  Result);
		}
		break;
	}

	APawn* TempPawn = CurrentPawn;

	CurrentController->ReceiveMoveCompleted.RemoveAll(this);
	CurrentPawn = nullptr;
	CurrentController = nullptr;

	OnMoveCompleted.Broadcast(TempPawn, Succ);
}
