#include "Info/GridInfo.h"
#include "Subsystems/GridSubsystem.h"

UGridInfo::UGridInfo()
{
}

UGridInfo::~UGridInfo()
{

}

void UGridInfo::Initialize(FHitResult _HitResult, UGrid* _Grid)
{
	HitResult = _HitResult;
	Grid = _Grid;
	Guid = FGuid::NewGuid();
}

void UGridInfo::BeginPlay_Implementation()
{
	
}

void UGridInfo::Clear_Implementation()
{
	HitResult.bBlockingHit = false;
	GameplayTags.Reset();
	Guid.Invalidate();
	PropertiesChanged();
}

void UGridInfo::SetHitResult(const FHitResult _HitResult)
{
	HitResult = _HitResult;
	PropertiesChanged();
}

FHitResult UGridInfo::GetHitResult() const
{
	return HitResult;
}

void UGridInfo::SetGameplayTags(const FGameplayTagContainer _GameplayTags)
{
	GameplayTags = _GameplayTags;
	PropertiesChanged();
}

FGameplayTagContainer UGridInfo::GetGameplayTags() const
{
	return GameplayTags;
}

void UGridInfo::SetGuid(const FGuid _Guid)
{
	Guid = _Guid;
	PropertiesChanged();
}

FGuid UGridInfo::GetGuid() const
{
	return Guid;
}

void UGridInfo::SetShortGuidSize(const int32 _ShortGuidSize)
{
	ShortGuidSize = _ShortGuidSize;
	PropertiesChanged();
}

int32 UGridInfo::GetShortGuidSize() const
{
	return ShortGuidSize;
}

FString UGridInfo::GetShortGuid() const
{
	return  Guid.ToString().Left(ShortGuidSize);
}

void UGridInfo::SetGrid(UGrid* _Grid)
{
	Grid = _Grid;
	PropertiesChanged();
}

UGrid* UGridInfo::GetGrid() const
{
	return Grid;
}

void UGridInfo::PropertiesChanged_Implementation()
{
	Grid->GridSubsystem->GetGridPainter()->UpdateGridState(Grid);
}
