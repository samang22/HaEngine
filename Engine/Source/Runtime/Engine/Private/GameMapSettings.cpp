#include "GameMapSettings.h"

UGameMapSettings::UGameMapSettings()
{
	if (!HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { return; }

	GConfig->LoadIniFile(GEngineIni, GBaseEngineIni, false);
	GConfig->LoadIniFile(GEngineIni, GDefaultEngineIni, true);

	FConfigFile& ConfigFile = GConfig->GetConfig(GEngineIni);

	FString String;
	ConfigFile.Get("/Script/EngineSettings.GameMapsSettings", "GameInstanceClass", String);

	GameInstanceClass = UClass::FindClass(String);
	UClass::GetAllSubclassOfClass(UObject::StaticClass());
}
