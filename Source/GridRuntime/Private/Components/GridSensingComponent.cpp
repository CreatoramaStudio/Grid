#include "Components/GridSensingComponent.h"

#include "EngineUtils.h"
#include "GridRuntimeLog.h"
#include "Util/GridUtilities.h"
#include "Subsystems/GridSubsystem.h"
#include "Subsystems/SquareGridSubsystem.h"
#include "TimerManager.h"

UGridSensingComponent::UGridSensingComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	bWantsInitializeComponent = true;
	bAutoActivate = false;

	SensingInterval = 0.5f;
	VisionGridRange = 0;

	VisionAngle = 180;
	bSkipLineSightTest = false;
	bDiagonal = false;

	SenseTestFunc.BindUFunction(this, "DefaultSenseTestFunc");
}

UGridSensingComponent::~UGridSensingComponent()
{

}

void UGridSensingComponent::InitializeComponent()
{
	Super::InitializeComponent();

	if (SensingInterval > 0.f)
	{
		const float InitialDelay = (SensingInterval * FMath::SRand()) + KINDA_SMALL_NUMBER;
		SetSensingTimer(InitialDelay);
	}
}

void UGridSensingComponent::SetSensingInterval(float NewInterval)
{
	if (FMath::Abs(SensingInterval - NewInterval) > FLT_EPSILON)
	{
		SensingInterval = NewInterval;

		if (IsValid(GetOwner()))
		{
			SetSensingTimer(SensingInterval);
		}
	}
}

bool UGridSensingComponent::CouldSeePawn(const APawn* Pawn) const
{
	return SensedPawns.Contains(Pawn);
}

void UGridSensingComponent::GetSensingGrids(TArray<UGrid*>& SensingGrids) const
{
	GetSensingGridsInternal(GetGridSubsystem(), SensingGrids);
}

void UGridSensingComponent::GetSensingGridsInternal(UGridSubsystem* GridSubsystem, TArray<UGrid*>& SensingGrids) const
{
	SensingGrids.Reset();

	if (GridSubsystem == nullptr)
	{
		PrintWarningGridRuntime("UGridSensingComponent::GetSensingGridsInternal GridSubsystem is nullptr.");
		return;
	}

	FVector SensorLocation;
	FRotator SensorRotation;

	GetOwner()->GetActorEyesViewPoint(SensorLocation, SensorRotation);

	USquareGridSubsystem* SquareGridMgr = Cast<USquareGridSubsystem>(GridSubsystem);
	if (SquareGridMgr != nullptr)
	{
		SquareGridMgr->GetSquareGridsByRange(SquareGridMgr->GetGridByPosition(SensorLocation), VisionGridRange, SensingGrids, bDiagonal);
	}
	else
	{
		GridSubsystem->GetGridsByRange(GridSubsystem->GetGridByPosition(SensorLocation), VisionGridRange, SensingGrids);
	}

	if (FMath::Abs(VisionAngle - 180.f) <= FLT_EPSILON)
		return;

	float VisionAngleCosine = FMath::Cos(FMath::DegreesToRadians(VisionAngle));
	FVector FacingDir = SensorRotation.Vector();

	SensingGrids.RemoveAll([&](UGrid* Grid)
	{
		FVector TestLocation(Grid->GetCenter());
		TestLocation.Z = SensorLocation.Z;
		return FVector::DotProduct((TestLocation - SensorLocation).GetSafeNormal(), FacingDir) + FLT_EPSILON < VisionAngleCosine;
	});
}

void UGridSensingComponent::DefaultSenseTestFunc(APawn* Pawn, bool& CouldSense)
{
	CouldSense = true;
}

void UGridSensingComponent::SetSensingTimer(float Interval)
{
	AActor* Owner = GetOwner();
	if (IsValid(Owner) && GEngine->GetNetMode(GetWorld()) < NM_Client)
	{
		Owner->GetWorldTimerManager().SetTimer(TimerHandle_Sensing, this, &UGridSensingComponent::OnSensingTimer, Interval, false);
	}
}

void UGridSensingComponent::OnSensingTimer()
{
	AActor* Owner = GetOwner();
	if (!IsValid(Owner) || !IsValid(Owner->GetWorld()))
		return;

	UpdateSensing();

	SetSensingTimer(SensingInterval);
}

void UGridSensingComponent::UpdateSensing()
{
	AActor* Owner = GetOwner();

	const TArray<APawn*> OldSenseResult = SensedPawns;
	SensedPawns.Reset();

	TArray<UGrid*> SensingGrids;

	GetSensingGrids(SensingGrids);

	for (int32 i = 0; i < SensingGrids.Num(); ++i)
	{
		APawn* Pawn = GetPawnByGrid(SensingGrids[i]);
		if (Pawn != nullptr && Pawn != Owner)
		{
			bool CouldSense = false;
			SenseTestFunc.Execute(Pawn, CouldSense);
			if (!CouldSense)
				continue;

			if (bSkipLineSightTest)
			{
				SensedPawns.Add(Pawn);
				continue;
			}

			AController* Controller = GetSensorController();
			if (Controller != nullptr && Controller->LineOfSightTo(Pawn, FVector::ZeroVector, true))
			{
				SensedPawns.Add(Pawn);
			}
		}
	}

	for (int32 i = 0; i < SensedPawns.Num(); ++i)
	{
		if (!OldSenseResult.Contains(SensedPawns[i]))
		{
			OnSeePawn.Broadcast(SensedPawns[i]);
		}
	}
}

AController* UGridSensingComponent::GetSensorController() const
{
	APawn* Pawn = Cast<APawn>(GetOwner());
	if (IsValid(Pawn) && IsValid(Pawn->Controller))
	{
		return Pawn->Controller;
	}
	return nullptr;
}

UGridSubsystem* UGridSensingComponent::GetGridSubsystem() const
{
	for (auto GridSubsystem : GetOwner()->GetWorld()->GetSubsystemArray<UGridSubsystem>())
	{
		if (GridSubsystem && GridSubsystem->IsInitialized())
		{
			return GridSubsystem;
		}
	}
	return nullptr;
}

APawn* UGridSensingComponent::GetPawnByGrid(UGrid* Grid) const
{
	AActor* Owner = GetOwner();
	UGridSubsystem* GridSubsystem = GetGridSubsystem();

	if (ensure(GridSubsystem != nullptr))
	{
		for (TActorIterator<APawn> Iterator(Owner->GetWorld()); Iterator; ++Iterator)
		{
			APawn* Pawn = *Iterator;
			if (Grid->Equal(GridSubsystem->GetGridByPosition(Pawn->GetActorLocation())))
			{
				return Pawn;
			}
		}
	}
	return nullptr;
}
