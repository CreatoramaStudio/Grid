#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GridSubsystem.h"
#include "Grids/HexagonGrid.h"
#include "HexagonGridSubsystem.generated.h"

USTRUCT()
struct FHexagonGridArray
{
	GENERATED_BODY()

	UPROPERTY()
	TArray<UHexagonGrid*> Grids;

	void Add(UHexagonGrid* Grid)
	{
		Grids.Add(Grid);
	}

	size_t Num() const
	{
		return Grids.Num();
	}

	UHexagonGrid* operator[](const std::size_t Idx)
	{
		return Grids[Idx];
	}

	const UHexagonGrid* operator[](const std::size_t Idx) const
	{
		return Grids[Idx];
	}
};

/**
 * reference: https://www.redblobgames.com/grids/hexagons/
 */
UCLASS()
class GRIDRUNTIME_API UHexagonGridSubsystem : public UGridSubsystem
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

	virtual void GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids) override;

	virtual void GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids) override;

	virtual void ClearAllGridInfo() override;

	UFUNCTION(BlueprintCallable, Category = "Hexagon")
	void GetHexagonGridsByCoord(const FIntVector& Coord, TArray<UHexagonGrid*>& Grids);

	UFUNCTION(BlueprintCallable, Category = "Hexagon")
	UHexagonGrid* GetHexagonGridByPosition(const FVector& Position);

protected:
	void CreateGrids(const FIntVector& Coord, FHexagonGridArray& GridArray);
	UHexagonGrid* CreateGrid(const FIntVector& Coord, const FHitResult& HitResult);

	static FIntVector CubeRound(float X, float Y, float Z);

protected:
	UPROPERTY()
	TMap<uint64, FHexagonGridArray> GridsPool;
};
