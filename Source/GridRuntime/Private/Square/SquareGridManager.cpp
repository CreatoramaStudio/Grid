#include "Square/SquareGridManager.h"

#include "GridPainter/GridDecalPainter.h"
#include "Util/GridUtilities.h"


void USquareGridManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void USquareGridManager::Deinitialize()
{
	Super::Deinitialize();
}

void USquareGridManager::SetGridSize(float NewSize)
{
	this->GridSize = NewSize;

	for (auto& Elem : GridsPool)
	{
		FSquareGridArray& GridArray = Elem.Value;
		for (int i = 0; i < GridArray.Grids.Num(); ++i)
		{
			GridArray[i]->SetGridSize(NewSize);
		}
	}
}

UGrid* USquareGridManager::GetGridByPosition(const FVector& Position)
{
	return GetSquareGridByPosition(Position);
}

void USquareGridManager::GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids)
{
	Grids.Reset();

	FSquareGridArray GridArray;
	GetSquareGridsByCoord(Coord, GridArray.Grids);
	
	for (int i = 0; i < GridArray.Num(); ++i)
	{
		Grids.AddUnique(GridArray[i]);
	}
}

void USquareGridManager::GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids)
{
	USquareGrid* MinGrid = Cast<USquareGrid>(GetGridByPosition(Bound.Min));
	USquareGrid* MaxGrid = Cast<USquareGrid>(GetGridByPosition(Bound.Max));

	FIntVector MinCoord = MinGrid->GetCoord();
	FIntVector MaxCoord = MaxGrid->GetCoord();

	TArray<UGrid*> TmpGrids;
	for (int i = MinCoord.X; i <= MaxCoord.X; ++i)
	{
		for (int j = MinCoord.Y; j <= MaxCoord.Y; ++j)
		{
			FIntVector CurrCoord(FIntVector(i, j, 0));

			GetGridsByCoord(CurrCoord, TmpGrids);

			Grids.Append(TmpGrids);
		}
	}
}

void USquareGridManager::GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids)
{
	GetSquareGridsByRange(Center, Range, Grids, false);
}

void USquareGridManager::GetSquareGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids, bool bDiagonal)
{
	Grids.Reset();

	if (Center == nullptr)
		return;

	FIntVector Coord = Center->Coord;

	TArray<UGrid*> TmpGrids;
	for (int i = -Range; i <= Range; ++i)
	{
		for (int j = -Range; j <= Range; ++j)
		{
			Coord.X = Center->Coord.X + i;
			Coord.Y = Center->Coord.Y + j;

			GetGridsByCoord(Coord, TmpGrids);

			for (int k = 0; k < TmpGrids.Num(); ++k)
			{
				UGrid* Grid = TmpGrids[k];

				if (bDiagonal || Grid->GetDistance(Center) <= Range)
				{
					Grids.Add(Grid);
				}
			}
		}
	}

	Grids.Sort([&](const UGrid& L, const UGrid& R)
	{
		return L.GetDistance(Center) < R.GetDistance(Center);
	});
}

void USquareGridManager::ClearAllGridInfo()
{
	for (auto& Elem : GridsPool)
	{
		FSquareGridArray& GridArray = Elem.Value;

		for (int i = 0; i< GridArray.Num(); ++i)
		{
			UGridInfo* GridInfo = GridArray[i]->GridInfo;

			if (GridInfo != nullptr)
			{
				GridInfo->Clear();
			}
		}
	}
}

void USquareGridManager::GetSquareGridsByCoord(const FIntVector& Coord, TArray<USquareGrid*>& Grids)
{
	Grids.Reset();

	uint64 GridUniqueId = UGridUtilities::GetUniqueIdByCoordinate(Coord);

	FSquareGridArray GridArray;
	if (GridsPool.Contains(GridUniqueId))
	{
		GridArray = GridsPool.FindChecked(GridUniqueId);
		Grids = GridArray.Grids;
	}
	else
	{
		CreateGrids(Coord, GridArray);

		GridsPool.Add(GridUniqueId, GridArray);

		Grids = GridArray.Grids;
	}
}

USquareGrid* USquareGridManager::GetSquareGridByPosition(const FVector& Position)
{
	FIntVector Coord(FMath::RoundToInt(Position.X / GridSize), FMath::RoundToInt(Position.Y / GridSize), 0);

	TArray<USquareGrid*> Grids;
	GetSquareGridsByCoord(Coord, Grids);

	USquareGrid* Rtn = nullptr;
	
	int MinDistance = TNumericLimits<int32>::Max();
	for (int i = 0; i < Grids.Num(); ++i)
	{
		USquareGrid* Grid = Grids[i];

		int Distance = FMath::Abs(Grid->Height - Position.Z);
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			Rtn = Grid;
		}
	}

	return Rtn;
}

void USquareGridManager::CreateGrids(const FIntVector& Coord, FSquareGridArray& GridArray)
{
	TArray<FHitResult> HitResults;
	FVector Center(Coord.X * GridSize, Coord.Y * GridSize, 0.f);

	LineTraceTest(Center, HitResults);

	for (int i = 0; i < HitResults.Num(); ++i)
	{
		if (i != 0)
		{
			// if this static mesh is too close to previous one, it has been blocked, skip this result
			FVector Oringin, Extent;
			HitResults[i - 1].Actor->GetActorBounds(true, Oringin, Extent);

			FBox Bound = FBox::BuildAABB(Oringin, Extent);

			FVector TestPoint = HitResults[i].ImpactPoint;

			TestPoint.Z += 10.f;
			if (Bound.IsInsideOrOn(TestPoint))
				continue;
		}
		GridArray.Add(CreateGrid(Coord, HitResults[i]));
	}

	if (GridArray.Num() == 0)
	{
		// create an empty grid
		GridArray.Add(CreateGrid(Coord, FHitResult()));
	}
}

USquareGrid* USquareGridManager::CreateGrid(const FIntVector& Coord, const FHitResult& HitResult)
{
	USquareGrid* Grid = NewObject<USquareGrid>(this, USquareGrid::StaticClass());
	check(Grid != nullptr);

	Grid->GridType = EGridType::Square;
	Grid->Coord = Coord;
	Grid->Height = HitResult.ImpactPoint.Z;
	Grid->GridInfo = NewObject<UGridInfo>(Grid, GridInfoClass);
	Grid->GridInfo->HitResult = HitResult;
	Grid->GridInfo->ParentGrid = Grid;
	Grid->GridManager = this;
	Grid->SetGridSize(GridSize);

	GridPainter->UpdateGridState(Grid);

	return Grid;
}
