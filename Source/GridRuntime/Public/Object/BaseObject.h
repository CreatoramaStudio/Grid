// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "BaseObject.generated.h"

/**
 * 
 */
UCLASS()
class GRIDRUNTIME_API UBaseObject : public UObject
{
	GENERATED_BODY()

	public:

	protected:

	private:

	public:
	
	protected:

	UFUNCTION(BlueprintCallable, Category = "BaseObject", meta = (Class = Actor), meta = (DeterminesOutputType = Class))
	AActor* SpawnActor(const TSubclassOf<AActor> Class, const FTransform Transform, const ESpawnActorCollisionHandlingMethod CollisionHandlingMethod,AActor* Owner,APawn* Instigator);

	private:
	
};
