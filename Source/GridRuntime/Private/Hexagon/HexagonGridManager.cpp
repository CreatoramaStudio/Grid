#include "Hexagon/HexagonGridManager.h"
#include "GridRuntimeLog.h"
#include "GridPainter/GridDecalPainter.h"
#include "Hexagon/HexagonPathFinder.h"
#include "Util/GridUtilities.h"

void UHexagonGridManager::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UHexagonGridManager::Deinitialize()
{
	Super::Deinitialize();	
}

void UHexagonGridManager::GetGridsByRange(UGrid* Center, int Range, TArray<UGrid*>& Grids)
{
	Grids.Reset();

	if (Center == nullptr)
		return;

	TArray<UGrid*> TmpGrids;

	for (int dx = -Range; dx <= Range; ++dx)
	{
		int Lowwer = FMath::Max(-Range, -dx - Range);
		int Upper = FMath::Min(Range, -dx + Range);
		for (int dy = Lowwer; dy <= Upper; ++dy)
		{
			int dz = -dx - dy;
			
			FIntVector Coord(Center->Coord.X + dx, Center->Coord.Y + dy, Center->Coord.Z + dz);

			GetGridsByCoord(Coord, TmpGrids);

			Grids.Append(TmpGrids);
		}
	}

	Grids.Sort([&](const UGrid& L, const UGrid& R)
	{
		return L.GetDistance(Center) < R.GetDistance(Center);
	});
}

void UHexagonGridManager::GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids)
{
	Grids.Reset();

	PrintErrorGridRuntime("AHexagonGridManager::GetGridsByBound not implemnt yet");
}



void UHexagonGridManager::SetGridSize(float NewSize)
{
	this->GridSize = NewSize;

	for (auto& Elem : GridsPool)
	{
		FHexagonGridArray& GridArray = Elem.Value;
		for (int i = 0; i < GridArray.Num(); ++i)
		{
			GridArray[i]->SetGridSize(NewSize);
		}
	}
}

void UHexagonGridManager::ClearAllGridInfo()
{
	for (auto& Elem : GridsPool)
	{
		FHexagonGridArray& GridArray = Elem.Value;;

		for (int i = 0; i < GridArray.Num(); ++i)
		{
			UGridInfo* GridInfo = GridArray[i]->GridInfo;

			if (GridInfo != nullptr)
			{
				GridInfo->Clear();
			}
		}
	}
}

void UHexagonGridManager::GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids)
{
	Grids.Reset();

	FHexagonGridArray GridArray;

	GetHexagonGridsByCoord(Coord, GridArray.Grids);

	for (int i = 0; i < GridArray.Num(); ++i)
	{
		Grids.Add(GridArray[i]);
	}
}

void UHexagonGridManager::GetHexagonGridsByCoord(const FIntVector& Coord, TArray<UHexagonGrid*>& Grids)
{
	Grids.Reset();

	check(Coord.X + Coord.Y + Coord.Z == 0);

	uint64 GridId = UGridUtilities::GetUniqueIdByCoordinate(Coord);

	FHexagonGridArray GridArray;
	if (GridsPool.Contains(GridId))
	{
		GridArray = GridsPool.FindChecked(GridId);
		Grids = GridArray.Grids;
	}
	else
	{
		CreateGrids(Coord, GridArray);
		Grids = GridArray.Grids;
		GridsPool.Add(GridId, GridArray);
	}
}

FIntVector UHexagonGridManager::CubeRound(float _X, float _Y, float _Z)
{
	int X = FPlatformMath::RoundToInt(_X);
	int Y = FPlatformMath::RoundToInt(_Y);
	int Z = FPlatformMath::RoundToInt(_Z);

	int dx = FMath::Abs(X - _X);
	int dy = FMath::Abs(Y - _Y);
	int dz = FMath::Abs(Z - _Z);

	if (dx > dy && dx > dz)
	{
		X = -Y - Z;
	}
	else if (dy > dz)
	{
		Y = -X - Z;
	}
	else
	{
		Z = -X - Y;
	}

	return FIntVector(X, Y, Z);
}

UGrid* UHexagonGridManager::GetGridByPosition(const FVector& Position)
{
	return GetHexagonGridByPosition(Position);
}

UHexagonGrid* UHexagonGridManager::GetHexagonGridByPosition(const FVector& Position)
{
	float X = (Position.X * FMath::Sqrt(3) / 3 - Position.Y / 3) / GridSize;
	float Z = Position.Y * 2 / 3 / GridSize;
	float Y = -X - Z;

	FIntVector Coord = CubeRound(X, Y, Z);

	TArray<UHexagonGrid*> Grids;
	GetHexagonGridsByCoord(Coord, Grids);

	UHexagonGrid* Rtn = nullptr;

	int MinDistance = TNumericLimits<int32>::Max();
	for (int i = 0; i < Grids.Num(); ++i)
	{
		UHexagonGrid* Grid = Grids[i];

		int Distance = FMath::Abs(Grid->Height - Position.Z);
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			Rtn = Grid;
		}
	}

	return Rtn;
}

void UHexagonGridManager::CreateGrids(const FIntVector& Coord, FHexagonGridArray& GridArray)
{
	TArray<FHitResult> HitResults;

	FVector Center(GridSize * FMath::Sqrt(3.f) * (Coord.X + Coord.Z / 2.f), GridSize * 3.f / 2.f * Coord.Z, 0.f);

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

UHexagonGrid* UHexagonGridManager::CreateGrid(const FIntVector& Coord, const FHitResult& HitResult)
{
	UHexagonGrid* Grid = NewObject<UHexagonGrid>(this, UHexagonGrid::StaticClass());
	check(Grid != nullptr);

	Grid->GridType = EGridType::Hexagon;
	Grid->Coord = Coord;
	Grid->Height = HitResult.ImpactPoint.Z;
	Grid->GridInfo = NewObject<UGridInfo>(Grid, GridInfoClass);
	Grid->GridInfo->ParentGrid = Grid;
	Grid->GridInfo->HitResult = HitResult;
	Grid->GridManager = this;
	Grid->SetGridSize(GridSize);

	GridPainter->UpdateGridState(Grid);

	return Grid;
}
