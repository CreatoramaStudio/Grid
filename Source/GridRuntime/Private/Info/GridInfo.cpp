#include "Info/GridInfo.h"
#include "Subsystems/GridSubsystem.h"

UGridInfo::UGridInfo()
{
}

UGridInfo::~UGridInfo()
{

}

void UGridInfo::BeginPlay_Implementation()
{
	
}

void UGridInfo::Clear_Implementation()
{
	HitResult.bBlockingHit = false;
	GameplayTags.Reset();
	Guid.Invalidate();
	Dirty();
}

void UGridInfo::Dirty()
{
	ParentGrid->GridSubsystem->GetGridPainter()->UpdateGridState(ParentGrid);
}
