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

FString FPaths::GetExtension(const FString& InPath, bool bIncludeDot)
{
    std::filesystem::path FilePath(InPath);
    FString Extension = FilePath.extension().wstring();
    if (!bIncludeDot)
    {
        Extension.erase(0, 1);
    }

    return Extension;
}