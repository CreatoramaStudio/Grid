#include "Subsystems/GridSubsystem.h"

#include "LogGridRuntime.h"
#include "GridPainter/GridDecalPainter.h"
#include "Kismet/KismetSystemLibrary.h"

void UGridSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
}

void UGridSubsystem::Deinitialize()
{
	DeinitializeManager();
}

void UGridSubsystem::InitializeManager(const TSubclassOf<UGridPathfinder> PathFinderClass, const TSubclassOf<UGridInfo> InfoClass, const TSubclassOf<UGridPainter> PainterClass, float gridSize, float TraceDistance)
{
	if (!GridPathFinderClass || !GridInfoClass || !GridPainterClass)
	{
		FLogGridRuntime::Warning("UGridSubsystem::InitializeManager Class is nullptr");
		return;
	}
	GridPathFinderClass = PathFinderClass;
	GridInfoClass = InfoClass;
	GridPainterClass = PainterClass;
	TraceTestDistance = TraceDistance;
	GridSize = gridSize;
	
	SetGridPainter(GridPainterClass);

	PathFinder = NewObject<UGridPathfinder>(this, GridPathFinderClass);
	PathFinder->GridSubsystem = this;

	bInitialized = true;
}

void UGridSubsystem::DeinitializeManager()
{
	if (PathFinder)
	{
		PathFinder->ConditionalBeginDestroy();
	}
	
	if (GridPainter)
	{
		GridPainter->ConditionalBeginDestroy();
	}
	

	bInitialized = false;
}

void UGridSubsystem::SetGridPainter(TSubclassOf<UGridPainter> PainterClass)
{
	GridPainterClass = PainterClass;
	
	if (GridPainter)
	{
		GridPainter->ConditionalBeginDestroy();
	}

	GridPainter = NewObject<UGridPainter>(this, GridPainterClass);
	check(GridPainter != nullptr);
	GridPainter->PostInitPainter();
	GridPainter->SetGridSubsystem(this);
}

UGridPainter* UGridSubsystem::GetGridPainter() const
{
	return GridPainter;
}

bool UGridSubsystem::IsInitialized() const
{
	return bInitialized;
}

void UGridSubsystem::LineTraceTest(const FVector& Center, TArray<FHitResult>& Results) const
{
	FVector Start = Center;
	FVector End = Start;

	Start.Z += TraceTestDistance / 2;
	End.Z -= TraceTestDistance / 2;

	const TArray<TEnumAsByte<EObjectTypeQuery> > ObjectTypes = { UEngineTypes::ConvertToObjectType(ECC_WorldStatic) };

	const TArray<AActor*> IgnoreActors;

	UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), Start, End, ObjectTypes, false, IgnoreActors, EDrawDebugTrace::None, Results, true);
}

void UGridSubsystem::GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids)
{
	Grids.Empty();
}

UGrid* UGridSubsystem::GetGridByCoord(const FIntVector& Coord)
{
	TArray<UGrid*> Grids;
	GetGridsByCoord(Coord, Grids);
	return Grids.Num() > 0 ? Grids[0] : nullptr;
}

UGrid* UGridSubsystem::GetGridByPosition(const FVector& Position)
{
	return nullptr;
}

void UGridSubsystem::ClearAllGridInfo()
{

}

UGridPathfinder* UGridSubsystem::GetPathFinder() const
{
	return PathFinder;
}

void UGridSubsystem::SetGridSize(const float NewSize)
{
	if (this->GridSize != NewSize)
	{
		this->GridSize = NewSize;
	}
}

float UGridSubsystem::GetGridSize() const
{
	return GridSize;
}

void UGridSubsystem::GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids)
{
	Grids.Reset();
}

void UGridSubsystem::GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids)
{
	Grids.Reset();
}

