#include "GridPainter/GridPainter.h"
#include "Engine/World.h"
#include "Subsystems/GridSubsystem.h"

UGridPainter::UGridPainter()
{
	GridSubsystem = nullptr;
	bIsTickable = true;
	TickInterval = 0.1f;
	LastTickTime = 0.f;
}

UGridPainter::~UGridPainter()
{

}

void UGridPainter::PostInitPainter()
{

}

void UGridPainter::SetGridSubsystem(UGridSubsystem* NewGridSubsystem)
{
	GridSubsystem = NewGridSubsystem;
}

void UGridPainter::Tick(float DeltaTime)
{
	if (GridSubsystem == nullptr)
		return;

	const float WorldTime = GridSubsystem->GetWorld()->GetTimeSeconds();

	if (WorldTime - LastTickTime >= TickInterval)
	{
		LastTickTime = WorldTime;
		TickImpl(DeltaTime);
	}
}

bool UGridPainter::IsTickable() const
{
	return bIsTickable;
}

TStatId UGridPainter::GetStatId() const
{
	return StatId;
}

void UGridPainter::UpdateGridState_Implementation(UGrid* Grid)
{
	check(Grid != nullptr);

	if (Grid->IsEmpty())
		return;

	if (Grid->bVisible)
	{
		VisibleGrids.AddUnique(Grid);
	}
	else
	{
		VisibleGrids.Remove(Grid);
	}
}

void UGridPainter::TickImpl_Implementation(float DeltaTime)
{

}
