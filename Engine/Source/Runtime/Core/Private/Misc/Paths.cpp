#include "Misc/Paths.h"

FString FPaths::EngineDir()
{
	static const FString Path = filesystem::current_path();
	return Path;
}

FString FPaths::EngineConfigDir()
{
	static const FString Path = EngineDir() + TEXT("\\Config");
	return Path;
}

FString FPaths::ProjectDLLDir()
{
	static const FString Path = EngineDir() + TEXT("\\Project");
	return Path;
}
