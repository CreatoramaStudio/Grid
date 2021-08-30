#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "GridAgents/GridNavigationAgent.h"
#include "DefaultGridNavigationAgent.generated.h"

/**
 *
 */
UCLASS()
class GRIDRUNTIME_API UDefaultGridNavigationAgent : public UGridNavigationAgent
{
	GENERATED_BODY()
public:

	UPROPERTY(EditDefaultsOnly,BlueprintReadWrite, Category = "DefaultGridNavigationAgent")
	float AcceptanceRadius;
	
protected:

	UPROPERTY(BlueprintReadOnly, Category = "DefaultGridNavigationAgent")
	APawn* CurrentPawn;

	UPROPERTY(BlueprintReadOnly, Category = "DefaultGridNavigationAgent")
	AAIController* CurrentController;

private:

public:
	
	UDefaultGridNavigationAgent();
	virtual ~UDefaultGridNavigationAgent() override;

	virtual bool Check_Implementation(APawn* Pawn, UGrid* From, UGrid* To) override;

	virtual bool RequestMove_Implementation(APawn* Pawn, UGrid* From, UGrid* To) override;

	virtual void StopMove_Implementation() override;

protected:

	UFUNCTION()
	virtual void OnAIControllerMoveCompleted(FAIRequestID RequestID, EPathFollowingResult::Type Result);
	
private:
};
