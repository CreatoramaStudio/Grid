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

	HexGridManager = nullptr;
	SquareGridManager = nullptr;

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
	
	FreeGridManager();

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
		UGridSubsystem* GridManager = GetGridManager();
		UGrid* Grid = GridManager->GetGridByPosition(Actor->GetActorLocation());
		
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

	UGridSubsystem* GridManager = GetGridManager();
	UGridPainter_Editor* DecalPainter = Cast<UGridPainter_Editor>(GridManager->GetGridPainter());
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

	FreeGridManager();
}

void FEdModeGridEditor::OnBeginPIE(bool bIsSimulating)
{
	if (!bIsSimulating)
	{
		FreeGridManager();
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

UGridSubsystem* FEdModeGridEditor::GetGridManager()
{
	if (CurrentModeName == FGridEditorCommands::SquareModeName)
	{
		if (!SquareGridManager)
		{
			SquareGridManager = GetEditorWorld()->GetSubsystem<USquareGridSubsystem>();
		}
		if (!SquareGridManager->IsInitialized())
		{
			SquareGridManager->GridPainterClass = UGridPainter_Editor::StaticClass();
			SquareGridManager->GridInfoClass = UGridInfo_Editor::StaticClass();
			//Actor::PostInitializeComponents wouldn't be called in editor mode, so we create grid painter manually
			
			SquareGridManager->InitializeManager(SquareGridManager->GridPathFinderClass,SquareGridManager->GridInfoClass,SquareGridManager->GridPainterClass,SquareGridManager->GridSize,SquareGridManager->TraceTestDistance);
			
			UpdateGridSettings();
		}

		return SquareGridManager;
	}
	
	if (CurrentModeName == FGridEditorCommands::HexagonModeName)
	{
		if (!HexGridManager)
		{
			HexGridManager = GetEditorWorld()->GetSubsystem<UHexagonGridSubsystem>();
		}
		if (!HexGridManager->IsInitialized())
		{
			HexGridManager->GridPainterClass = UGridPainter_Editor::StaticClass();
			HexGridManager->GridInfoClass = UGridInfo_Editor::StaticClass();
			//Actor::PostInitializeComponents wouldn't be called in editor mode, so we create grid painter manually
			
			HexGridManager->InitializeManager(HexGridManager->GridPathFinderClass,HexGridManager->GridInfoClass,HexGridManager->GridPainterClass,HexGridManager->GridSize,HexGridManager->TraceTestDistance);
			
			UpdateGridSettings();
		}

		return HexGridManager;
	}
	
	return nullptr;
}

void FEdModeGridEditor::FreeGridManager()
{
	ClearVisibleGrids();

	HexGridManager = nullptr;

	SquareGridManager = nullptr;
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
	UGridSubsystem* GridManager = GetGridManager();
	UGridPainter_Editor* DecalPainter = Cast<UGridPainter_Editor>(GridManager->GetGridPainter());

	if (DecalPainter != nullptr)
	{
		DecalPainter->DefaultDecalMaterial = SquareGridSettings->DecalMaterial;
		DecalPainter->SquareNormalMaterial = SquareGridSettings->DecalMaterial;
		DecalPainter->SquareSensingMaterial = SquareGridSettings->GridSensingVisualizerMaterial;
	}
	GridManager->SetGridSize(SquareGridSettings->GridSize);

	if (SquareGridSettings->bShowGrids && GEditor->GetActiveViewport())
	{
		UGrid* Center = GridManager->GetGridByPosition(SquareGridSettings->GridCenter);

		GridManager->GetGridsByRange(Center, SquareGridSettings->GridShowRange, VisibleGrids);
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
	UGridSubsystem* GridManager = GetGridManager();
	UGridPainter_Editor* DecalPainter = Cast<UGridPainter_Editor>(GridManager->GetGridPainter());

	if (DecalPainter != nullptr)
	{
		DecalPainter->DefaultDecalMaterial = HexagonGridSettings->DecalMaterial;
		DecalPainter->HexNormalMaterial = HexagonGridSettings->DecalMaterial;
		DecalPainter->HexSensingMaterial = HexagonGridSettings->GridSensingVisualizerMaterial;
	}
	GridManager->SetGridSize(HexagonGridSettings->GridSize);

	if (HexagonGridSettings->bShowGrids && GEditor->GetActiveViewport())
	{
		UGrid* Center = GridManager->GetGridByPosition(HexagonGridSettings->GridCenter);

		GridManager->GetGridsByRange(Center, HexagonGridSettings->GridShowRange, VisibleGrids);
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
