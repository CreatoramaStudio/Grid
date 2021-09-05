#include "Grids/Grid.h"
#include "Subsystems/GridSubsystem.h"
#include "GridPainter/GridPainter.h"
#include "Components/DecalComponent.h"

UGrid::UGrid()
{
	GridSize = 0.f;
	Height = 0.f;

	bVisible = false;

	GridInfo = nullptr;
	GridSubsystem = nullptr;
	GridType = EGridType::Unknown;
}

UGrid::~UGrid()
{

}

void UGrid::BeginPlay_Implementation()
{
	
}

FVector UGrid::GetCenter() const
{
	return FVector::ZeroVector;
}

void UGrid::SetGridSize(float Size)
{
	GridSize = Size;

	GridSubsystem->GetGridPainter()->UpdateGridState(this);
}

float UGrid::GetGridSize() const
{
	return GridSize;
}

bool UGrid::Equal(const UGrid* R) const
{
	return Coord == R->Coord && FMath::Abs(Height - R->Height) <= FLT_EPSILON;
}

void UGrid::SetVisibility(const bool bNewVisibility)
{
	if (bVisible != bNewVisibility)
	{
		bVisible = bNewVisibility;
		GridSubsystem->GetGridPainter()->UpdateGridState(this);
		OnVisibilityChanged.Broadcast(bVisible);
	}
}

bool UGrid::GetVisibility() const
{
	return bVisible;
}

int UGrid::GetDistance(const UGrid* Dest) const
{
	return 0;
}

bool UGrid::IsEmpty() const
{
	return !GridInfo->GetHitResult().bBlockingHit;
}

FIntVector UGrid::GetCoord() const
{
	return Coord;
}

void UGrid::GetNeighbors_Implementation(TArray<UGrid*>& Grids)
{
	Grids.Reset();
}
