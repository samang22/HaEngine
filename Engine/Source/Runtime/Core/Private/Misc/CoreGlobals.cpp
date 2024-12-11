#include "CoreTypes.h"
#include "Modules/ModuleManager.h"
#include "GenericPlatform/GenericPlatformTime.h"

class FCoreModule : public FDefaultModuleImpl
{
public:
	virtual void StartupModule() override
	{
		int a = 0;
	}

	virtual bool SupportsDynamicReloading() override
	{
		// Core cannot be unloaded or reloaded
		return false;
	}
};


IMPLEMENT_MODULE(FCoreModule, Core);