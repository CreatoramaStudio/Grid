#pragma once

class UGrid;
class UGridPathfinder;

struct FComparer
{
	TMap<UGrid*, int32>* FCost;
	
	bool operator()(const UGrid& L, const UGrid& R) const
	{
		const int32 LFCost = FCost->Contains(&L) ? FCost->FindChecked(&L) : TNumericLimits<int32>::Max();
		const int32 RFCost = FCost->Contains(&R) ? FCost->FindChecked(&R) : TNumericLimits<int32>::Max();
		return LFCost < RFCost;
	}
};

class FAStar
{
public:
	FAStar(UGrid* _Start, UGrid* _Goal, UGridPathfinder* _PathFinder, bool _Reversed = false);

	bool Step();

	UGrid* GetIntersect(const FAStar& R);

	void CollectPath(UGrid* StartGrid, TArray<UGrid*>& Result);

	bool bSuccess;
private:
	
	FComparer Comparer;

	bool Reversed;
	TArray<UGrid*> OpenSet;
	TSet<UGrid*> CloseSet;
	TMap<UGrid*, int32> FCost;
	TMap<UGrid*, int32> GCost;
	TMap<UGrid*, UGrid*> CameFrom;
	UGrid* Start;
	UGrid* Goal;
	UGridPathfinder* PathFinder;

	friend class FBidirectionalAStar;
};

class FBidirectionalAStar
{
public:
	FBidirectionalAStar(UGrid* _Start, UGrid* _Goal, UGridPathfinder* _PathFinder);

	bool Step();

	void CollectPath(TArray<UGrid*>& Result);

	bool bSuccess;
private:
	UGrid * IntersectGrid;
	FAStar ForwardAStar;
	FAStar BackwardAStar;
};
