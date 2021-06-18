#include "Square/SquarePathFinder.h"
#include "Grid.h"

USquarePathFinder::USquarePathFinder()
{
}

USquarePathFinder::~USquarePathFinder()
{
}

int32 USquarePathFinder::GetCost_Implementation(UGrid* From, UGrid* To)
{
	return FMath::Abs(From->Coord.X - To->Coord.X) + FMath::Abs(From->Coord.Y - To->Coord.Y);
}
