#include "GridRuntimeLog.h"

DEFINE_LOG_CATEGORY(LogGridRuntime);

void PrintInfoGridRuntime(const FString& String)
{
	UE_LOG(LogGridRuntime, Display, TEXT("%s"), *String);
}

void PrintWarningGridRuntime(const FString& String)
{
	UE_LOG(LogGridRuntime, Warning, TEXT("%s"), *String);
}

void PrintErrorGridRuntime(const FString& String)
{
	UE_LOG(LogGridRuntime, Error, TEXT("%s"), *String);
}
