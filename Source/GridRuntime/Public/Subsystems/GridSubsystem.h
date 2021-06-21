#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "GridPathfinders/GridPathfinder.h"
#include "GridPainter/GridPainter.h"
#include "Grids/Grid.h"
#include "GridPainter/GridDecalPainter.h"

#include "GridSubsystem.generated.h"

/**
*
*/
UCLASS(Abstract)
class GRIDRUNTIME_API UGridSubsystem : public UWorldSubsystem 
{
	GENERATED_BODY()

public:

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual void InitializeManager(TSubclassOf<UGridPathfinder> PathFinderClass, TSubclassOf<UGridInfo> InfoClass, TSubclassOf<UGridPainter> PainterClass,float gridSize = 100,float TraceDistance = 10000);

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	virtual void DeinitializeManager();

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
	UGridPathfinder* GetPathFinder() const;

	UGridPainter* GetGridPainter() const;

	UFUNCTION(BlueprintCallable, Category = "GridManager")
	bool IsInitialized() const;

	void LineTraceTest(const FVector& Center, TArray<FHitResult>& Results) const;

	//////////////////////////////////////////////////////////////////////////
	// properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridManager")
	TSubclassOf<UGridPathfinder> GridPathFinderClass = UGridPathfinder::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridManager")
	TSubclassOf<UGridInfo> GridInfoClass = UGridInfo::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridManager")
	TSubclassOf<UGridPainter> GridPainterClass = UGridDecalPainter::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridManager")
	float TraceTestDistance = 10000;	

	void SetGridPainter(TSubclassOf<UGridPainter> PainterClass);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn=true), Category = "GridManager")
	float GridSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridManager")
	bool bInitialized = false;

	UPROPERTY()
	UGridPainter* GridPainter;

	UPROPERTY()
	UGridPathfinder* PathFinder;

#ifdef WITH_EDITOR
	friend class FEdModeGridEditor;
#endif
};
