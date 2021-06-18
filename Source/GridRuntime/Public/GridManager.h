#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridPathfindingParams.h"
#include "GridPainter/GridPainter.h"
#include "Grid.h"
#include "GridPainter/GridDecalPainter.h"

#include "GridManager.generated.h"

/**
*
*/
UCLASS(Abstract)
class GRIDRUNTIME_API UGridManager : public UWorldSubsystem 
{
	GENERATED_BODY()

public:

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual void InitializeManager(TSubclassOf<UGridPathFinder> PathFinderClass, TSubclassOf<UGridInfo> InfoClass, TSubclassOf<UGridPainter> PainterClass);

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual void SetGridSize(float NewSize);

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual float GetGridSize() const;

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual void GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids);

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual void GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids);

	/** Get all grids belong to this coord */
	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual void GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids);

	/** if you don't need multi-level grid support, this function can be used, otherwise using GetGridsByCoord*/
	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual UGrid* GetGridByCoord(const FIntVector& Coord);

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual UGrid* GetGridByPosition(const FVector& Position);

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual void ClearAllGridInfo();

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	UGridPathFinder* GetPathFinder() const;

	//////////////////////////////////////////////////////////////////////////
	// properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn=true), Category = "GridManager")
	TSubclassOf<UGridPathFinder> GridPathFinderClass = UGridPathFinder::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn=true), Category = "GridManager")
	TSubclassOf<UGridInfo> GridInfoClass = UGridInfo::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn=true), Category = "GridManager")
	TSubclassOf<UGridPainter> GridPainterClass = UGridDecalPainter::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn = true), Category = "GridManager")
	float TraceTestDistance = 10000;

	void SetGridPainter(TSubclassOf<UGridPainter> PainterClass);

	UGridPainter* GetGridPainter() const;

	void LineTraceTest(const FVector& Center, TArray<FHitResult>& Results) const;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn=true), Category = "GridManager")
	float GridSize = 100;

	UPROPERTY()
	UGridPainter* GridPainter;

	UPROPERTY()
	UGridPathFinder* PathFinder;

#ifdef WITH_EDITOR
	friend class FEdModeGridEditor;
#endif
};
