#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GridInfo.generated.h"

class UGrid;

/**
 * 
 */
UCLASS(Blueprintable)
class GRIDRUNTIME_API UGridInfo : public UObject
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite, Category = "GridInfo")
	FHitResult HitResult;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridInfo")
	FGameplayTagContainer GameplayTags;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GridInfo")
	FGuid Guid;

	UPROPERTY(BlueprintReadOnly, Category = "GridInfo")
	UGrid* ParentGrid;

protected:

private:

public:
	UGridInfo();
	virtual ~UGridInfo() override;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridInfo")
	void BeginPlay();
	virtual void BeginPlay_Implementation();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "GridInfo")
	void Clear();
	virtual void Clear_Implementation();

	/**
	property has changed, notify GridPainter refresh grid state
	*/
	UFUNCTION(BlueprintCallable, Category = "GridInfo")
	void Dirty();
	  
protected:
	  
private:  
	

	
};
