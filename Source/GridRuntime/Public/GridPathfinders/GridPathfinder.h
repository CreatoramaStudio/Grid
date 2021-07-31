#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GridPathfinder.generated.h"

class UGrid;
class UGridSubsystem;

USTRUCT(BlueprintType)
struct GRIDRUNTIME_API FGridPathfindingRequest
{
	GENERATED_BODY()

public:
	FGridPathfindingRequest();

	UPROPERTY(BlueprintReadWrite, Category = "GridPathFindingRequest")
	AActor* Sender;

	UPROPERTY(BlueprintReadWrite, Category = "GridPathFindingRequest")
	UGrid* Start;

	UPROPERTY(BlueprintReadWrite, Category = "GridPathFindingRequest")
	UGrid* Destination;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "GridPathFindingRequest")
	int32 MaxCost;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "GridPathFindingRequest")
	int32 MaxSearchStep;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "GridPathFindingRequest")
	bool bRemoveDest;

	UPROPERTY(BlueprintReadWrite, AdvancedDisplay, Category = "GridPathFindingRequest")
	FGameplayTagContainer ExtraTags;
};

/**
	Default pathfinder using manhattan distance to calculate cost.
	Inherit from USquarePathFinder or UHexagonPathFinder to customize pathfinding
*/
UCLASS(Blueprintable)
class GRIDRUNTIME_API UGridPathfinder : public UObject
{
	GENERATED_BODY()

public:
	UGridPathfinder();
	virtual ~UGridPathfinder() override;

	UFUNCTION(BlueprintCallable, Category = "GridPathFinder")
	UGrid* GetStart() const;

	UFUNCTION(BlueprintCallable, Category = "GridPathFinder")
	UGrid* GetDestination() const;

	UFUNCTION(BlueprintCallable, Category = "GridPathFinder")
	AActor* GetSender() const;

	UFUNCTION(BlueprintCallable, Category = "GridPathFinder")
	UGridSubsystem* GetGridSubsystem() const;

	UFUNCTION(BlueprintCallable, Category = "GridPathFinder")
	const FGameplayTagContainer& GetExtraTags() const;

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	bool FindPath(const FGridPathfindingRequest& request, TArray<UGrid*>& Result);

	UFUNCTION(BlueprintCallable, Category = "Utilities")
	bool GetReachableGrids(AActor* Sender,const int32 MaxCost, TArray<UGrid*>& Result);

	/** If return false, the pawn can't move from 'Start' to 'Dest', the path has been blocked, and the cost will be ignore */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPathFinder")
	bool IsReachable(UGrid* Start, UGrid* Dest);
	virtual bool IsReachable_Implementation(UGrid* Start, UGrid* Dest);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPathFinder")
	int32 GetCost(UGrid* From, UGrid* To);
	virtual int32 GetCost_Implementation(UGrid* From, UGrid* To) { return 1; };

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPathFinder")
	int32 Heuristic(UGrid* From, UGrid* To);
	virtual int32 Heuristic_Implementation(UGrid* From, UGrid* To);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPathFinder")
	void Reset();
	virtual void Reset_Implementation() {};

	FGridPathfindingRequest Request;

	UPROPERTY()
	UGridSubsystem* GridSubsystem;
};
