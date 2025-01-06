#pragma once
#include "CoreTypes.h"
#include "Private/Misc/inicpp.h"

class CORE_API FConfigFile
{
public:
	bool Load(FStringView InPath);
	bool TryOverride(FStringView InPath);

private:
	bool bLoaded = false;
	bool bOverrided = false;

	ini::IniFile File;
	ini::IniFile OverrideFile;
};