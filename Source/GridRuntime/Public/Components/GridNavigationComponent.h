#pragma once

#include "CoreMinimal.h"
#include "AIController.h"
#include "Components/ActorComponent.h"
#include "GridAgents/GridNavigationAgent.h"
#include "GridNavigationComponent.generated.h"

class UGrid;
class UGridSubsystem;

UENUM(BlueprintType)
enum class EGridNavMode : uint8
{
	GridBased,
	Free,
};

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FGridNavEventSignature, UGridNavigationComponent*, Component);

UCLASS(ClassGroup=(Grid), Blueprintable, meta=(BlueprintSpawnableComponent))
class GRIDRUNTIME_API UGridNavigationComponent : public UActorComponent
{
	GENERATED_BODY()

public:

	UPROPERTY(BlueprintAssignable, Category = "GridNavigationComponent")
	FGridNavEventSignature OnArrivalNewGrid;

	UPROPERTY(BlueprintAssignable, Category = "GridNavigationComponent")
	FGridNavEventSignature OnArrivalGoal;

	/** if you implement a new GridNavigationAgent, add that class to this Array */
	UPROPERTY(EditDefaultsOnly,BlueprintReadOnly, Category = "GridNavigationComponent")
	TArray<TSubclassOf<UGridNavigationAgent>> AgentClasses;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GridNavigationComponent")
	EGridNavMode NavMode;

protected:

	UPROPERTY(BlueprintReadOnly, Category = "GridNavigationComponent")
	APawn* OwnerPawn;

	UPROPERTY(BlueprintReadOnly, Category = "GridNavigationComponent")
	AAIController* OwnerController;

	UPROPERTY(BlueprintReadOnly, Category = "GridNavigationComponent")
	TArray<UGrid*> CurrentFollowingPath;

	UPROPERTY(BlueprintReadOnly, Category = "GridNavigationComponent")
	TArray<UGridNavigationAgent*> Agents;

	UPROPERTY(BlueprintReadOnly, Category = "GridNavigationComponent")
	UGridNavigationAgent* CurrentAgent;

	int FollowingPathIndex;

	bool bIsMoving;
	
private:

public:

	UGridNavigationComponent();
	virtual ~UGridNavigationComponent() override;

	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "GridNavigationComponent")
	virtual UGrid* GetOwnerGridPosition();

	UFUNCTION(BlueprintCallable, Category = "GridNavigationComponent")
	virtual void GetReachableGridsByRange(const int32 Range, TArray<UGrid*>& Grids);

	/**
	*	@note make sure character's "Max Acceleration" large enough for smooth moving
	@note if PathFinder == null, GridSubsystem's default PathFinder will be used
	*/
	UFUNCTION(BlueprintCallable, Category = "GridNavigationComponent")
	virtual bool RequestMove(UGrid* DestGrid);

	UFUNCTION(BlueprintCallable, Category = "GridNavigationComponent")
	virtual void StopMove();

	UFUNCTION(BlueprintCallable, Category = "GridNavigationComponent")
	virtual bool IsMoving() const;

protected:

	bool MoveToNext();
	virtual bool MoveToNextGrid();
	virtual bool MoveToNextPoint();

	UGridNavigationAgent* FindAgent(UGrid* Start, UGrid* Goal);

	UFUNCTION()
	virtual void OnMoveCompleted(APawn* Pawn, bool bSuccess);

	virtual UGridSubsystem* GetGridSubsystem() const;

private:
};
