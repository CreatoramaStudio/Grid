#include "GridPathfinders/HexagonGridPathfinder.h"
#include "Grids/Grid.h"

UHexagonGridPathfinder::UHexagonGridPathfinder()
{

}

UHexagonGridPathfinder::~UHexagonGridPathfinder()
{

}

int32 UHexagonGridPathfinder::GetCost_Implementation(UGrid* From, UGrid* To)
{
	return (FMath::Abs(From->Coord.X - To->Coord.X) + FMath::Abs(From->Coord.Y - To->Coord.Y) + FMath::Abs(From->Coord.Z - To->Coord.Z)) / 2;
}
