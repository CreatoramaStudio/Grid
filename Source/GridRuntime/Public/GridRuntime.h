#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"

/**
 * The public interface to this module
 */
class FGridRuntimeModule : public IModuleInterface
{
public:

	/** IModuleInterface implementation */
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;
};

