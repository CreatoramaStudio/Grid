#include "GridPainter/GridDecalPainter.h"
#include "Util/GridUtilities.h"
#include "Subsystems/GridSubsystem.h"
#include "Components/DecalComponent.h"

UGridDecalPainter::UGridDecalPainter()
{
	DefaultDecalMaterial = nullptr;
}

UGridDecalPainter::~UGridDecalPainter()
{

}

void UGridDecalPainter::TickImpl_Implementation(float DeltaTime)
{
	// for (auto VisibleGrid : VisibleGrids)
	// {
	// 	UpdateDecal(VisibleGrid);
	// }
}

void UGridDecalPainter::UpdateGridState_Implementation(UGrid* Grid)
{
	Super::UpdateGridState_Implementation(Grid);

	UpdateDecal(Grid);
}

void UGridDecalPainter::UpdateDecal(UGrid* Grid)
{
	if (!Grid->GridInfo->GetHitResult().bBlockingHit)
	{
		return;
	}

	UDecalComponent* DecalComp;

	if (Grid2Decal.Contains(Grid))
	{
		DecalComp = Grid2Decal.FindChecked(Grid);
	}
	else
	{
		DecalComp = NewObject<UDecalComponent>(this);
		DecalComp->RegisterComponentWithWorld(GridSubsystem->GetWorld());
		DecalComp->SetRelativeRotation(FRotator(90.0f, 0.0f, 0.0f).Quaternion());
		Grid2Decal.Add(Grid, DecalComp);
	}

	DecalComp->SetVisibility(Grid->GetVisibility());

	DecalComp->SetDecalMaterial(GetDecalMaterial(Grid));

	DecalComp->DecalSize = UGridUtilities::CalcGridDecalSize(Grid->GridType, Grid->GetGridSize()) * GridDecalSizeOffset;

	DecalComp->SetWorldLocation(Grid->GetCenter());
}

UMaterialInterface* UGridDecalPainter::GetDecalMaterial_Implementation(UGrid* Grid)
{
	return DefaultDecalMaterial;
}

