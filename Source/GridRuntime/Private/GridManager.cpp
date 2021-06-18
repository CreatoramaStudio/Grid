#include "GridManager.h"
#include "GridPainter/GridDecalPainter.h"
#include "Kismet/KismetSystemLibrary.h"

void UGridManager::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UGridManager::Deinitialize()
{
}

void UGridManager::InitializeManager(const TSubclassOf<UGridPathFinder> PathFinderClass, const TSubclassOf<UGridInfo> InfoClass, const TSubclassOf<UGridPainter> PainterClass, float gridSize, float TraceDistance)
{
	GridPathFinderClass = PathFinderClass;
	GridInfoClass = InfoClass;
	GridPainterClass = PainterClass;
	TraceTestDistance = TraceDistance;
	GridSize = gridSize;
	
	SetGridPainter(GridPainterClass);

	PathFinder = NewObject<UGridPathFinder>(this, GridPathFinderClass);
	PathFinder->GridManager = this;
}

void UGridManager::SetGridPainter(TSubclassOf<UGridPainter> PainterClass)
{
	GridPainterClass = PainterClass;
	if (GridPainter != nullptr)
		GridPainter->ConditionalBeginDestroy();

	GridPainter = NewObject<UGridPainter>(this, GridPainterClass);
	check(GridPainter != nullptr);
	GridPainter->PostInitPainter();
	GridPainter->SetGridManager(this);
}

UGridPainter* UGridManager::GetGridPainter() const
{
	return GridPainter;
}

void UGridManager::LineTraceTest(const FVector& Center, TArray<FHitResult>& Results) const
{
	FVector Start = Center;
	FVector End = Start;

	Start.Z += TraceTestDistance / 2;
	End.Z -= TraceTestDistance / 2;

	const TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic) };

	const TArray<AActor*> IgnoreActors;

	UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), Start, End, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, Results, true);
}

void UGridManager::GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids)
{
	Grids.Empty();
}

UGrid* UGridManager::GetGridByCoord(const FIntVector& Coord)
{
	TArray<UGrid*> Grids;
	GetGridsByCoord(Coord, Grids);
	return Grids.Num() > 0 ? Grids[0] : nullptr;
}

UGrid* UGridManager::GetGridByPosition(const FVector& Position)
{
	return nullptr;
}

void UGridManager::ClearAllGridInfo()
{

}

UGridPathFinder* UGridManager::GetPathFinder() const
{
	return PathFinder;
}

void UGridManager::SetGridSize(const float NewSize)
{
	if (this->GridSize != NewSize)
	{
		this->GridSize = NewSize;
	}
}

float UGridManager::GetGridSize() const
{
	return GridSize;
}

void UGridManager::GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids)
{
	Grids.Reset();
}

void UGridManager::GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids)
{
	Grids.Reset();
}

