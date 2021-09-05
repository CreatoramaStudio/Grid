// Fill out your copyright notice in the Description page of Project Settings.


#include "Object/BaseObject.h"

AActor* UBaseObject::SpawnActor(const TSubclassOf<AActor> Class, const FTransform Transform, const ESpawnActorCollisionHandlingMethod CollisionHandlingMethod, AActor* Owner, APawn* Instigator)
{
	FActorSpawnParameters Parameters;
	Parameters.Owner = Owner;
	Parameters.Instigator = Instigator;
	Parameters.SpawnCollisionHandlingOverride = CollisionHandlingMethod;
	return  GetWorld()->SpawnActor<AActor>(Class,Transform,Parameters);
}
