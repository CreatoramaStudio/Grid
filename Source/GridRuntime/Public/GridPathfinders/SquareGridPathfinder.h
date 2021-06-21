#pragma once

#include "CoreMinimal.h"
#include "GridPathfinder.h"
#include "SquareGridPathfinder.generated.h"

UCLASS()
class GRIDRUNTIME_API USquareGridPathfinder : public UGridPathfinder
{
	GENERATED_BODY()

public:
	USquareGridPathfinder();
	virtual ~USquareGridPathfinder() override;

	virtual int32 GetCost_Implementation(UGrid* From, UGrid* To) override;
};
