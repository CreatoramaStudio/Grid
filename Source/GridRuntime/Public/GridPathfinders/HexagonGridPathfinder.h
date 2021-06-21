#pragma once

#include "CoreMinimal.h"
#include "GridPathfinder.h"
#include "HexagonGridPathfinder.generated.h"

UCLASS()
class GRIDRUNTIME_API UHexagonGridPathfinder : public UGridPathfinder
{
	GENERATED_BODY()

public:
	UHexagonGridPathfinder();
	virtual ~UHexagonGridPathfinder() override;

	virtual int32 GetCost_Implementation(UGrid* From, UGrid* To) override;
};
