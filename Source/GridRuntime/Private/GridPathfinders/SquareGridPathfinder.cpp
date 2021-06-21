#include "GridPathfinders/SquareGridPathfinder.h"
#include "Grids/Grid.h"

USquareGridPathfinder::USquareGridPathfinder()
{
}

USquareGridPathfinder::~USquareGridPathfinder()
{
}

int32 USquareGridPathfinder::GetCost_Implementation(UGrid* From, UGrid* To)
{
	return FMath::Abs(From->Coord.X - To->Coord.X) + FMath::Abs(From->Coord.Y - To->Coord.Y);
}
