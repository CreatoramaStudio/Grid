#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Object/BaseObject.h"
#include "GridInfo.generated.h"

class UGrid;

/**
 * 
 */
UCLASS(Blueprintable)
class GRIDRUNTIME_API UGridInfo : public UBaseObject
{
	GENERATED_BODY()
	
public:

protected:

	UPROPERTY()
	UGrid* Grid;

	FHitResult HitResult;

	FGuid Guid;

	UPROPERTY(EditAnywhere, Category = "GridInfo")
	int32 ShortGuidSize = 5;
	
	UPROPERTY(EditAnywhere, Category = "GridInfo")
	FGameplayTagContainer GameplayTags;

private:

public:
	
	UGridInfo();
	
	virtual ~UGridInfo() override;

	void Initialize(FHitResult _HitResult,UGrid* _Grid);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridInfo")
	void BeginPlay();
	virtual void BeginPlay_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridInfo")
	void Clear();
	virtual void Clear_Implementation();

	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void SetHitResult(const FHitResult _HitResult);

	UFUNCTION(BlueprintPure, Category = "GridInfo")
	FHitResult GetHitResult() const;

	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void SetGameplayTags(const FGameplayTagContainer _GameplayTags);

	UFUNCTION(BlueprintPure, Category = "GridInfo")
	FGameplayTagContainer GetGameplayTags() const;

	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void SetGuid(const FGuid _Guid);

	UFUNCTION(BlueprintPure, Category = "GridInfo")
	FGuid GetGuid() const;

	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void SetShortGuidSize(const int32 _ShortGuidSize);

	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	int32 GetShortGuidSize() const;

	UFUNCTION(BlueprintPure, Category = "GridInfo")
	FString GetShortGuid() const;

	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void SetGrid(UGrid* _Grid);

	UFUNCTION(BlueprintPure, Category = "GridInfo")
	UGrid* GetGrid() const;

	/**
	property has changed, notify GridPainter refresh grid state
	*/
	UFUNCTION(BlueprintNativeEvent,BlueprintCallable, Category = "GridInfo")
	void PropertiesChanged();
	virtual void PropertiesChanged_Implementation();
	  
protected:
	  
private:  
	

	
};
