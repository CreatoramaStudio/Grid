#include "Subsystems/HexagonGridSubsystem.h"
#include "GridRuntimeLog.h"
#include "GridPainter/GridDecalPainter.h"
#include "Util/GridUtilities.h"

void UHexagonGridSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void UHexagonGridSubsystem::Deinitialize()
{
	Super::Deinitialize();	
}

void UHexagonGridSubsystem::GetGridsByRange(UGrid* Center, const int Range, TArray<UGrid*>& Grids)
{
	Grids.Reset();

	if (Center == nullptr)
		return;

	for (int DX = -Range; DX <= Range; ++DX)
	{
		const int Lower = FMath::Max(-Range, -DX - Range);
		const int Upper = FMath::Min(Range, -DX + Range);
		for (int Dy = Lower; Dy <= Upper; ++Dy)
		{
			TArray<UGrid*> TmpGrids;
			const int Dz = -DX - Dy;
			
			FIntVector Coord(Center->Coord.X + DX, Center->Coord.Y + Dy, Center->Coord.Z + Dz);

			GetGridsByCoord(Coord, TmpGrids);

			Grids.Append(TmpGrids);
		}
	}

	Grids.Sort([&](const UGrid& L, const UGrid& R)
	{
		return L.GetDistance(Center) < R.GetDistance(Center);
	});
}

void UHexagonGridSubsystem::GetGridsByBound(const FBox& Bound, TArray<UGrid*>& Grids)
{
	Grids.Reset();

	PrintErrorGridRuntime("AHexagonGridSubsystem::GetGridsByBound not implement yet");
}



void UHexagonGridSubsystem::SetGridSize(const float NewSize)
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

void UHexagonGridSubsystem::ClearAllGridInfo()
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

void UHexagonGridSubsystem::GetGridsByCoord(const FIntVector& Coord, TArray<UGrid*>& Grids)
{
	Grids.Reset();

	FHexagonGridArray GridArray;

	GetHexagonGridsByCoord(Coord, GridArray.Grids);

	for (int i = 0; i < GridArray.Num(); ++i)
	{
		Grids.Add(GridArray[i]);
	}
}

void UHexagonGridSubsystem::GetHexagonGridsByCoord(const FIntVector& Coord, TArray<UHexagonGrid*>& Grids)
{
	Grids.Reset();

	check(Coord.X + Coord.Y + Coord.Z == 0);

	const uint64 GridId = UGridUtilities::GetUniqueIdByCoordinate(Coord);

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

FIntVector UHexagonGridSubsystem::CubeRound(const float _X, const float _Y, const float _Z)
{
	int X = FPlatformMath::RoundToInt(_X);
	int Y = FPlatformMath::RoundToInt(_Y);
	int Z = FPlatformMath::RoundToInt(_Z);

	const int DX = FMath::Abs(X - _X);
	const int Dy = FMath::Abs(Y - _Y);
	const int Dz = FMath::Abs(Z - _Z);

	if (DX > Dy && DX > Dz)
	{
		X = -Y - Z;
	}
	else if (Dy > Dz)
	{
		Y = -X - Z;
	}
	else
	{
		Z = -X - Y;
	}

	return FIntVector(X, Y, Z);
}

UGrid* UHexagonGridSubsystem::GetGridByPosition(const FVector& Position)
{
	return GetHexagonGridByPosition(Position);
}

UHexagonGrid* UHexagonGridSubsystem::GetHexagonGridByPosition(const FVector& Position)
{
	const float X = (Position.X * FMath::Sqrt(3) / 3 - Position.Y / 3) / GridSize;
	const float Z = Position.Y * 2 / 3 / GridSize;
	const float Y = -X - Z;

	const FIntVector Coord = CubeRound(X, Y, Z);

	TArray<UHexagonGrid*> Grids;
	GetHexagonGridsByCoord(Coord, Grids);

	UHexagonGrid* Rtn = nullptr;

	int MinDistance = TNumericLimits<int32>::Max();
	for (int i = 0; i < Grids.Num(); ++i)
	{
		UHexagonGrid* Grid = Grids[i];

		const int Distance = FMath::Abs(Grid->Height - Position.Z);
		if (Distance < MinDistance)
		{
			MinDistance = Distance;
			Rtn = Grid;
		}
	}

	return Rtn;
}

void UHexagonGridSubsystem::CreateGrids(const FIntVector& Coord, FHexagonGridArray& GridArray)
{
	TArray<FHitResult> HitResults;

	const FVector Center(GridSize * FMath::Sqrt(3.f) * (Coord.X + Coord.Z / 2.f), GridSize * 3.f / 2.f * Coord.Z, 0.f);

	LineTraceTest(Center, HitResults);

	for (int i = 0; i < HitResults.Num(); ++i)
	{
		if (i != 0)
		{
			// if this static mesh is too close to previous one, it has been blocked, skip this result
			FVector Origin, Extent;
			HitResults[i - 1].Actor->GetActorBounds(true, Origin, Extent);

			FBox Bound = FBox::BuildAABB(Origin, Extent);

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

UHexagonGrid* UHexagonGridSubsystem::CreateGrid(const FIntVector& Coord, const FHitResult& HitResult)
{
	UHexagonGrid* Grid = NewObject<UHexagonGrid>(this, UHexagonGrid::StaticClass());
	check(Grid != nullptr);

	Grid->GridType = EGridType::Hexagon;
	Grid->Coord = Coord;
	Grid->Height = HitResult.ImpactPoint.Z;
	Grid->GridInfo = NewObject<UGridInfo>(Grid, GridInfoClass);
	Grid->GridInfo->ParentGrid = Grid;
	Grid->GridInfo->HitResult = HitResult;
	Grid->GridSubsystem = this;
	Grid->SetGridSize(GridSize);

	GridPainter->UpdateGridState(Grid);

	return Grid;
}
