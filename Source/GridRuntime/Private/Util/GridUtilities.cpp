#include "Util/GridUtilities.h"
#include "LogGridRuntime.h"
#include "Subsystems/GridSubsystem.h"


uint64 UGridUtilities::GetGridUniqueIdByCoordinate(const FIntVector& Coord)
{
	return ((uint64)Coord.Z << 44) + ((uint64)Coord.Y << 22) + (uint64)Coord.X;
}

uint64 UGridUtilities::GetGridUniqueId(const UGrid* Grid)
{
	return GetGridUniqueIdByCoordinate(Grid->Coord);
}

FString UGridUtilities::GetGridUniqueIdString(const UGrid* Grid)
{
	return FString::Printf(TEXT("%llu"), GetGridUniqueId(Grid));
}

FVector UGridUtilities::CalcGridDecalSize(const EGridType GridType, const float GridSize)
{
	switch (GridType)
	{
	case EGridType::Square:
		{
			FVector DecalSize(GridSize, GridSize, GridSize);

			DecalSize /= FVector(4.f, 2.f, 2.f);

			return DecalSize;
		}
	case EGridType::Hexagon:
		{
			FVector DecalSize(GridSize, GridSize * 2, GridSize * FMath::Sqrt(3));

			DecalSize /= 2;

			return DecalSize;
		}
	default:
		FLogGridRuntime::Error("UGridUtilities::CalcGridDecalSize unknown grid type");
		return FVector::ZeroVector;
	}
}

FIntVector UGridUtilities::Subtract_IntVectorIntVector(const FIntVector& L, const FIntVector& R)
{
	return L - R;
}

FIntVector UGridUtilities::Add_IntVectorIntVector(const FIntVector& L, const FIntVector& R)
{
	return L + R;
}

FIntVector UGridUtilities::Multiply_IntVectorInt(const FIntVector& L, int R)
{
	return L * R;
}

bool UGridUtilities::EqualEqual_GridGrid(const UGrid* A, const UGrid* B)
{
	return A->Equal(B);
}

bool UGridUtilities::NotEqual_GridGrid(const UGrid* A, const UGrid* B)
{
	return !A->Equal(B);
}

bool UGridUtilities::GridTraceSingleForObjects(const UGrid* Grid, const float TraceDistance, const TArray<TEnumAsByte<EObjectTypeQuery> > & ObjectTypes, const bool bTraceComplex, const TArray<AActor*>& ActorsToIgnore, const EDrawDebugTrace::Type DrawDebugType, FHitResult& OutHit, const bool bIgnoreSelf, const FLinearColor TraceColor, const FLinearColor TraceHitColor, const float DrawTime)
{
	const FVector Start = Grid->GetCenter() + FVector(0.f, 0.f, TraceDistance / 2.f);
	const FVector End = Grid->GetCenter() - FVector(0.f, 0.f, TraceDistance / 2.f);

	return UKismetSystemLibrary::LineTraceSingleForObjects(Grid->GridSubsystem->GetWorld(), Start, End, ObjectTypes
		, bTraceComplex, ActorsToIgnore, DrawDebugType, OutHit
		, bIgnoreSelf, TraceColor, TraceHitColor, DrawTime);
}

void UGridUtilities::GetBoundsByGridArray(const TArray<UGrid*>& Grids, FBoxSphereBounds& Bounds)
{
	if (Grids.Num() == 0)
		return;

	Bounds = Grids[0]->Bounds;

	for (int i = 1; i < Grids.Num(); ++i)
	{
		Bounds = Bounds + Grids[i]->Bounds;
	}
}
