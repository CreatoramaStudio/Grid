#pragma once

#include "CoreMinimal.h"
#include "Grids/SquareGrid.h"
#include "Subsystems/GridSubsystem.h"
#include "SquareGridSubsystem.generated.h"

USTRUCT()
struct FSquareGridArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<USquareGrid*> Grids;

	void Add(USquareGrid* Grid)
	{
		Grids.Add(Grid);
	}

	size_t Num() const
	{
		return Grids.Num();
	}

	USquareGrid* operator[](const std::size_t Idx)
	{
		return Grids[Idx];
	}

	const USquareGrid* operator[](const std::size_t Idx) const
	{
		return Grids[Idx];
	}
};

/**
*
*/
UCLASS()
class GRIDRUNTIME_API USquareGridSubsystem : public UGridSubsystem
{
	GENERATED_BODY()

public:

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	virtual void SetGridSize(float NewSize) override;

	virtual UGrid* GetGridByPosition(const FVector& Position) override;

	virtual void GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids) override;

	virtual void GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids) override;

	virtual void GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids) override;

	virtual void ClearAllGridInfo() override;

	UFUNCTION(BlueprintCallable, Category = "SquareGridSubsystem")
	void GetSquareGridsByCoord(const FIntVector& Coord, TArray<USquareGrid*>& Grids);

	UFUNCTION(BlueprintCallable, Category = "SquareGridSubsystem")
	void GetSquareGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids, bool bDiagonal = false);

	UFUNCTION(BlueprintCallable, Category = "SquareGridSubsystem")
	USquareGrid* GetSquareGridByPosition(const FVector& Position);

protected:
	void CreateGrids(const FIntVector& Coord, FSquareGridArray& GridArray);
	USquareGrid* CreateGrid(const FIntVector& Coord, const FHitResult& HitResult);

protected:
	UPROPERTY()
	TMap<uint64, FSquareGridArray> GridsPool;
};
