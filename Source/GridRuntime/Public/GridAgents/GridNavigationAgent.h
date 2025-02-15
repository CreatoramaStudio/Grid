#pragma once

#include "CoreMinimal.h"
#include "Object/BaseObject.h"
#include "UObject/NoExportTypes.h"
#include "GridNavigationAgent.generated.h"

class UGrid;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FGridNavAgentEvent, APawn*, Pawn, bool, Succ);

/**
 * GridNavigationAgent is used to control the pawn's movement behavior.
   inherit from this class to implement special movement(jump, climb, etc)
 */
UCLASS(Blueprintable, abstract)
class GRIDRUNTIME_API UGridNavigationAgent : public UBaseObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintAssignable, BlueprintCallable, Category = "GridNavigationAgent")
	FGridNavAgentEvent OnMoveCompleted;

protected:

private:

public:

	UGridNavigationAgent();
	virtual ~UGridNavigationAgent() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPainter")
	void BeginPlay();
	virtual void BeginPlay_Implementation();

	/** check if this agent can process this move */
	UFUNCTION(BlueprintNativeEvent, Category = "GridNavigationAgent")
	bool Check(APawn* Pawn, UGrid* From, UGrid* To);
	virtual bool Check_Implementation(APawn* Pawn, UGrid* From, UGrid* To);

	/** if you override RequestMove, when the pawn arrival destination, you should call OnMoveCompleted */
	UFUNCTION(BlueprintNativeEvent, Category = "GridNavigationAgent")
	bool RequestMove(APawn* Pawn, UGrid* From, UGrid* To);
	virtual bool RequestMove_Implementation(APawn* Pawn, UGrid* From, UGrid* To);

	UFUNCTION(BlueprintNativeEvent, Category = "GridNavigationAgent")
	void StopMove();
	virtual void StopMove_Implementation();

protected:

private:
};
