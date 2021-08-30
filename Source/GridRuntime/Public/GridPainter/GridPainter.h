#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Tickable.h"
#include "Grids/Grid.h"
#include "GridPainter.generated.h"

/**
 * GridPainter is used for rendering grid in game world, the default painter is GridDecalPainter.
   Inherit from this class to customize grid rendering
 */
UCLASS(Blueprintable, abstract)
class GRIDRUNTIME_API UGridPainter : public UObject,public FTickableGameObject
{
	GENERATED_BODY()

public:
	
	UPROPERTY(BlueprintReadOnly, Category = "GridPainter")
	UGridSubsystem* GridSubsystem;

	UPROPERTY(BlueprintReadWrite, Category = "GridPainter")
	TArray<UGrid*> VisibleGrids;

protected:
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GridPainter")
	bool bIsTickable;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "GridPainter")
	float TickInterval;

	float LastTickTime;

	TStatId StatId;

private:

public:

	UGridPainter();
	virtual ~UGridPainter() override;

	virtual void PostInitPainter();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPainter")
	void BeginPlay();
	virtual void BeginPlay_Implementation();
	

	virtual void SetGridSubsystem(UGridSubsystem* NewGridSubsystem);

	virtual void Tick(float DeltaTime) override;

	virtual bool IsTickable() const override;

	virtual TStatId GetStatId() const override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridPainter")
	void UpdateGridState(UGrid* Grid);
	virtual void UpdateGridState_Implementation(UGrid* Grid);

	UFUNCTION(BlueprintNativeEvent, Category = "GridPainter")
	void TickImpl(float DeltaTime);
	virtual void TickImpl_Implementation(float DeltaTime);

protected:

private:
};
