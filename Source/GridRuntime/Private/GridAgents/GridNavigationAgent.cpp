#include "GridAgents/GridNavigationAgent.h"

UGridNavigationAgent::UGridNavigationAgent()
{

}

UGridNavigationAgent::~UGridNavigationAgent()
{
}

void UGridNavigationAgent::BeginPlay_Implementation()
{
}

bool UGridNavigationAgent::Check_Implementation(APawn* Pawn, UGrid* From, UGrid* To)
{
	return false;
}

bool UGridNavigationAgent::RequestMove_Implementation(APawn* Pawn, UGrid* From, UGrid* To)
{
	return false;
}

void UGridNavigationAgent::StopMove_Implementation()
{

}
