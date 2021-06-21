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

	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	virtual void InitializeManager(TSubclassOf<UGridPathfinder> PathFinderClass, TSubclassOf<UGridInfo> InfoClass, TSubclassOf<UGridPainter> PainterClass,float gridSize = 100,float TraceDistance = 10000);

	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	virtual void DeinitializeManager();

	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	virtual void SetGridSize(float NewSize);

	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	virtual float GetGridSize() const;

	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	virtual void GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids);

	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	virtual void GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids);

	/** Get all grids belong to this coord */
	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	virtual void GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids);

	/** if you don't need multi-level grid support, this function can be used, otherwise using GetGridsByCoord*/
	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	virtual UGrid* GetGridByCoord(const FIntVector& Coord);

	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	virtual UGrid* GetGridByPosition(const FVector& Position);

	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	virtual void ClearAllGridInfo();

	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	UGridPathfinder* GetPathFinder() const;

	UGridPainter* GetGridPainter() const;

	UFUNCTION(BlueprintCallable, Category = "GridSubsystem")
	bool IsInitialized() const;

	void LineTraceTest(const FVector& Center, TArray<FHitResult>& Results) const;

	//////////////////////////////////////////////////////////////////////////
	// properties
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSubsystem")
	TSubclassOf<UGridPathfinder> GridPathFinderClass = UGridPathfinder::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSubsystem")
	TSubclassOf<UGridInfo> GridInfoClass = UGridInfo::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSubsystem")
	TSubclassOf<UGridPainter> GridPainterClass = UGridDecalPainter::StaticClass();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSubsystem")
	float TraceTestDistance = 10000;	

	void SetGridPainter(TSubclassOf<UGridPainter> PainterClass);

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Meta = (ExposeOnSpawn=true), Category = "GridSubsystem")
	float GridSize = 100;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridSubsystem")
	bool bInitialized = false;

	UPROPERTY()
	UGridPainter* GridPainter;

	UPROPERTY()
	UGridPathfinder* PathFinder;

#ifdef WITH_EDITOR
	friend class FEdModeGridEditor;
#endif
};
