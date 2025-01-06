#pragma once
#include "CoreTypes.h"

class CORE_API FPaths
{
public:
	// exe 경로
	static FString EngineDir();
	static FString EngineConfigDir();
	static FString ProjectDLLDir();
};