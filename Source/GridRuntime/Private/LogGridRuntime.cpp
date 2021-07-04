// Fill out your copyright notice in the Description page of Project Settings.

#include "LogGridRuntime.h"

DEFINE_LOG_CATEGORY(LogGridRuntime);

void FLogGridRuntime::Info(const FString& String)
{
	UE_LOG(LogGridRuntime, Display, TEXT("%s"), *String);
}

void FLogGridRuntime::Warning(const FString& String)
{
	UE_LOG(LogGridRuntime, Warning, TEXT("%s"), *String);
}

void FLogGridRuntime::Error(const FString& String)
{
	UE_LOG(LogGridRuntime, Error, TEXT("%s"), *String);
}