#include "GridPainter/GridOutlinePainter.h"
#include "Subsystems/GridSubsystem.h"

UGridOutlinePainter::UGridOutlinePainter()
{
	bIsTickable = true;

	GridOutlineComp = CreateDefaultSubobject<UGridOutlineComponent>("GridOutlineComp");

	OutlineThickness = 5.f;
	ZOffset = 5.f;
}

UGridOutlinePainter::~UGridOutlinePainter()
{

}

void UGridOutlinePainter::SetGridSubsystem(UGridSubsystem* NewGridSubsystem)
{
	Super::SetGridSubsystem(NewGridSubsystem);

	if (!GridOutlineComp->IsRegistered())
	{
		GridOutlineComp->RegisterComponentWithWorld(GridSubsystem->GetWorld());
	}
}

void UGridOutlinePainter::TickImpl_Implementation(float DeltaTime)
{
	if (GridOutlineComp)
	{
		GridOutlineComp->UpdateGridInfo();
	}
}

void UGridOutlinePainter::GetColorPriority_Implementation(TArray<FLinearColor>& Colors)
{
	Colors.Reset();
	Colors.Add(FLinearColor::White);
}

void UGridOutlinePainter::GetColors_Implementation(UGrid* Grid, TArray<FLinearColor>& Colors)
{
	Colors.Reset();
	Colors.Add(FLinearColor::White);
}
