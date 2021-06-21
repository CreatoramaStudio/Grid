#include "Info/GridInfo.h"
#include "Subsystems/GridSubsystem.h"

UGridInfo::UGridInfo()
{
}

UGridInfo::~UGridInfo()
{

}

void UGridInfo::Clear_Implementation()
{
	HitResult.bBlockingHit = false;
	GameplayTags.Reset();
	Dirty();
}

void UGridInfo::Dirty()
{
	ParentGrid->GridSubsystem->GetGridPainter()->UpdateGridState(ParentGrid);
}
