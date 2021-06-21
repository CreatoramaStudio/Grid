// Copyright 1998-2016 Epic Games, Inc. All Rights Reserved.

// You should place include statements to your module's private header files here.  You only need to
// add includes for headers that are used in most of your module's source files though.
#pragma once
#include "IGridEditor.h"

#include "Grids/Grid.h"
#include "Subsystems/GridSubsystem.h"

#include "Grids/HexagonGrid.h"
#include "Subsystems/HexagonGridSubsystem.h"

#include "Grids/SquareGrid.h"
#include "Subsystems/SquareGridSubsystem.h"
#include "GridPathfinders/SquareGridPathfinder.h"

#define LOG_INFO(FMT, ...) UE_LOG(GridEditor, Display, (FMT), ##__VA_ARGS__)
#define LOG_WARNING(FMT, ...) UE_LOG(GridEditor, Warning, (FMT), ##__VA_ARGS__)
#define LOG_ERROR(FMT, ...) UE_LOG(GridEditor, Error, (FMT), ##__VA_ARGS__)

extern const FEditorModeID EM_GridEditor;
