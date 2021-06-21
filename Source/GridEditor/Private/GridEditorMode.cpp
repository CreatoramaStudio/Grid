#include "GridEditorMode.h"
#include "GridEditorPCH.h"
#include "Editor/UnrealEdEngine.h"
#include "UnrealEdGlobals.h"
#include "Toolkits/ToolkitManager.h"
#include "EditorModeManager.h"
#include "GridEditorModeToolkit.h"
#include "GridEditorCommands.h"
#include "Misc/FileHelper.h"
#include "Components/GridSensingComponent.h"
#include "ComponentVisualizers/GridSensingComponentVisualizer.h"
#include "GridPainter_Editor.h"
#include "GridInfo_Editor.h"

#define LOCTEXT_NAMESPACE "GridEditorMode" 

FEdModeGridEditor::FEdModeGridEditor()
{
	bEditorDelegateRegistered = false;

	HexGridSubsystem = nullptr;
	SquareGridSubsystem = nullptr;

	SquareGridSettings = NewObject<USquareGridSettings>(GetTransientPackage(), TEXT("SquareGridSettings"), RF_Transactional);
	SquareGridSettings->SetParent(this);

	HexagonGridSettings = NewObject<UHexagonGridSettings>(GetTransientPackage(), TEXT("HexagonGridSettings"), RF_Transactional);
	HexagonGridSettings->SetParent(this);

	GetSensingCompVisualizer()->GridEditor = this;
}

FEdModeGridEditor::~FEdModeGridEditor()
{
	if (bEditorDelegateRegistered)
	{
		FEditorDelegates::NewCurrentLevel.RemoveAll(this);
		FEditorDelegates::MapChange.RemoveAll(this);
		FEditorDelegates::BeginPIE.RemoveAll(this);
		FEditorDelegates::EndPIE.RemoveAll(this);
	}
}

bool FEdModeGridEditor::UsesToolkits() const
{
	return true;
}

void FEdModeGridEditor::Enter()
{
	FEdMode::Enter();

	if (!Toolkit.IsValid())
	{
		Toolkit = MakeShareable(new FGridEditorToolkit(this));
		Toolkit->Init(Owner->GetToolkitHost());
	}

	SetCurrentMode(FGridEditorCommands::SquareModeName);

	if (!bEditorDelegateRegistered)
	{
		FEditorDelegates::NewCurrentLevel.AddSP(this, &FEdModeGridEditor::OnNewCurrentLevel);
		FEditorDelegates::MapChange.AddSP(this, &FEdModeGridEditor::OnMapChanged);
		FEditorDelegates::BeginPIE.AddSP(this, &FEdModeGridEditor::OnBeginPIE);
		FEditorDelegates::EndPIE.AddSP(this, &FEdModeGridEditor::OnEndPIE);

		bEditorDelegateRegistered = true;
	}

	GetSensingCompVisualizer()->SetEnabled(true);

	UpdateGridSettings();
}

void FEdModeGridEditor::Exit()
{
	GetSensingCompVisualizer()->SetEnabled(false);

	UpdateGridSettings();
	
	FreeGridSubsystem();

	if (Toolkit.IsValid())
	{
		FToolkitManager::Get().CloseToolkit(Toolkit.ToSharedRef());
		Toolkit.Reset();
	}

	FEdMode::Exit();
}

void FEdModeGridEditor::Tick(FEditorViewportClient* ViewportClient, float DeltaTime)
{
	FEdMode::Tick(ViewportClient, DeltaTime);
}

void FEdModeGridEditor::ActorMoveNotify()
{
	AActor* Actor = GetFirstSelectedActorInstance();

	if (Actor != nullptr && CurrentModeName == FGridEditorCommands::HexagonModeName && HexagonGridSettings->bGridSnap)
	{
		UGridSubsystem* GridSubsystem = GetGridSubsystem();
		UGrid* Grid = GridSubsystem->GetGridByPosition(Actor->GetActorLocation());
		
		if (Grid != nullptr)
		{
			FVector NewLocation = Actor->GetActorLocation();
			const FVector GridCenter = Grid->GetCenter();
			NewLocation.X = GridCenter.X;
			NewLocation.Y = GridCenter.Y;

			Actor->SetActorLocation(NewLocation);
		}
	}
}

void FEdModeGridEditor::ActorPropChangeNotify()
{
}

void FEdModeGridEditor::ActorSelectionChangeNotify()
{
	AActor* SelectedActor = GetFirstSelectedActorInstance();
	if (SelectedActor == nullptr)
		return;

	UGridSensingComponent* SensingComp = Cast<UGridSensingComponent>(SelectedActor->GetComponentByClass(UGridSensingComponent::StaticClass()));
	if (SensingComp == nullptr)
	{
		GetSensingCompVisualizer()->ClearSensingGrids();
	}
}

void FEdModeGridEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FEdMode::AddReferencedObjects(Collector);

	Collector.AddReferencedObject(SquareGridSettings);
	Collector.AddReferencedObject(HexagonGridSettings);
}

void FEdModeGridEditor::SetCurrentMode(FName ModeName)
{
	CurrentModeName = ModeName;

	UGridSubsystem* GridSubsystem = GetGridSubsystem();
	UGridPainter_Editor* DecalPainter = Cast<UGridPainter_Editor>(GridSubsystem->GetGridPainter());
	DecalPainter->bIsSquareGrid = CurrentModeName == FGridEditorCommands::SquareModeName;

	UpdateGridSettings();
}

FName FEdModeGridEditor::GetCurrentMode()
{
	return CurrentModeName;
}

void FEdModeGridEditor::OnSettingsChanged(const FPropertyChangedEvent& PropertyChangedEvent)
{
	UpdateGridSettings();
}

void FEdModeGridEditor::OnNewCurrentLevel()
{
}

void FEdModeGridEditor::OnMapChanged(uint32 Event)
{
	GetSensingCompVisualizer()->SensingGrids.Reset();

	VisibleGrids.Reset();

	FreeGridSubsystem();
}

void FEdModeGridEditor::OnBeginPIE(bool bIsSimulating)
{
	if (!bIsSimulating)
	{
		FreeGridSubsystem();
	}
}

void FEdModeGridEditor::OnEndPIE(bool bIsSimulating)
{
	if (!bIsSimulating)
	{
		UpdateGridSettings();
	}
}

UWorld* FEdModeGridEditor::GetEditorWorld()
{
	return GEditor->GetEditorWorldContext().World();
}

UGridSubsystem* FEdModeGridEditor::GetGridSubsystem()
{
	if (CurrentModeName == FGridEditorCommands::SquareModeName)
	{
		if (!SquareGridSubsystem)
		{
			SquareGridSubsystem = GetEditorWorld()->GetSubsystem<USquareGridSubsystem>();
		}
		if (!SquareGridSubsystem->IsInitialized())
		{
			SquareGridSubsystem->GridPainterClass = UGridPainter_Editor::StaticClass();
			SquareGridSubsystem->GridInfoClass = UGridInfo_Editor::StaticClass();
			//Actor::PostInitializeComponents wouldn't be called in editor mode, so we create grid painter manually
			
			SquareGridSubsystem->InitializeManager(SquareGridSubsystem->GridPathFinderClass,SquareGridSubsystem->GridInfoClass,SquareGridSubsystem->GridPainterClass,SquareGridSubsystem->GridSize,SquareGridSubsystem->TraceTestDistance);
			
			UpdateGridSettings();
		}

		return SquareGridSubsystem;
	}
	
	if (CurrentModeName == FGridEditorCommands::HexagonModeName)
	{
		if (!HexGridSubsystem)
		{
			HexGridSubsystem = GetEditorWorld()->GetSubsystem<UHexagonGridSubsystem>();
		}
		if (!HexGridSubsystem->IsInitialized())
		{
			HexGridSubsystem->GridPainterClass = UGridPainter_Editor::StaticClass();
			HexGridSubsystem->GridInfoClass = UGridInfo_Editor::StaticClass();
			//Actor::PostInitializeComponents wouldn't be called in editor mode, so we create grid painter manually
			
			HexGridSubsystem->InitializeManager(HexGridSubsystem->GridPathFinderClass,HexGridSubsystem->GridInfoClass,HexGridSubsystem->GridPainterClass,HexGridSubsystem->GridSize,HexGridSubsystem->TraceTestDistance);
			
			UpdateGridSettings();
		}

		return HexGridSubsystem;
	}
	
	return nullptr;
}

void FEdModeGridEditor::FreeGridSubsystem()
{
	ClearVisibleGrids();

	HexGridSubsystem = nullptr;

	SquareGridSubsystem = nullptr;
}

void FEdModeGridEditor::ClearVisibleGrids()
{
	for (int i = 0; i < VisibleGrids.Num(); ++i)
	{
		UGrid* Grid = VisibleGrids[i];
		UGridInfo_Editor* GridInfo = Cast<UGridInfo_Editor>(Grid->GridInfo);
		GridInfo->SetShowNormal(false);
		Grid->SetVisibility(GridInfo->GetVisibility());
	}
	VisibleGrids.Reset();
}

void FEdModeGridEditor::UpdateGridSettings()
{
	ClearVisibleGrids();

	if (CurrentModeName == FGridEditorCommands::SquareModeName)
	{
		UpdateSquareSettings();
	}
	else if (CurrentModeName == FGridEditorCommands::HexagonModeName)
	{
		UpdateHexagonSettings();
	}
}

void FEdModeGridEditor::UpdateSquareSettings()
{
	UGridSubsystem* GridSubsystem = GetGridSubsystem();
	UGridPainter_Editor* DecalPainter = Cast<UGridPainter_Editor>(GridSubsystem->GetGridPainter());

	if (DecalPainter != nullptr)
	{
		DecalPainter->DefaultDecalMaterial = SquareGridSettings->DecalMaterial;
		DecalPainter->SquareNormalMaterial = SquareGridSettings->DecalMaterial;
		DecalPainter->SquareSensingMaterial = SquareGridSettings->GridSensingVisualizerMaterial;
	}
	GridSubsystem->SetGridSize(SquareGridSettings->GridSize);

	if (SquareGridSettings->bShowGrids && GEditor->GetActiveViewport())
	{
		UGrid* Center = GridSubsystem->GetGridByPosition(SquareGridSettings->GridCenter);

		GridSubsystem->GetGridsByRange(Center, SquareGridSettings->GridShowRange, VisibleGrids);
		for (int i = 0; i < VisibleGrids.Num(); ++i)
		{
			UGrid* Grid = VisibleGrids[i];
			UGridInfo_Editor* GridInfo = Cast<UGridInfo_Editor>(Grid->GridInfo);
			GridInfo->SetShowNormal(true);
			Grid->SetVisibility(GridInfo->GetVisibility());
		}
	}
}

void FEdModeGridEditor::UpdateHexagonSettings()
{
	UGridSubsystem* GridSubsystem = GetGridSubsystem();
	UGridPainter_Editor* DecalPainter = Cast<UGridPainter_Editor>(GridSubsystem->GetGridPainter());

	if (DecalPainter != nullptr)
	{
		DecalPainter->DefaultDecalMaterial = HexagonGridSettings->DecalMaterial;
		DecalPainter->HexNormalMaterial = HexagonGridSettings->DecalMaterial;
		DecalPainter->HexSensingMaterial = HexagonGridSettings->GridSensingVisualizerMaterial;
	}
	GridSubsystem->SetGridSize(HexagonGridSettings->GridSize);

	if (HexagonGridSettings->bShowGrids && GEditor->GetActiveViewport())
	{
		UGrid* Center = GridSubsystem->GetGridByPosition(HexagonGridSettings->GridCenter);

		GridSubsystem->GetGridsByRange(Center, HexagonGridSettings->GridShowRange, VisibleGrids);
		for (int i = 0; i < VisibleGrids.Num(); ++i)
		{
			UGrid* Grid = VisibleGrids[i];
			UGridInfo_Editor* GridInfo = Cast<UGridInfo_Editor>(Grid->GridInfo);
			GridInfo->SetShowNormal(true);
			Grid->SetVisibility(GridInfo->GetVisibility());
		}
	}
}

TSharedPtr<FGridSensingComponentVisualizer> FEdModeGridEditor::GetSensingCompVisualizer() const
{
	return StaticCastSharedPtr<FGridSensingComponentVisualizer>(GUnrealEd->FindComponentVisualizer(UGridSensingComponent::StaticClass()->GetFName()));
}

#undef LOCTEXT_NAMESPACE
