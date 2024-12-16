#include "LaunchEngineLoop.h"
#include "Engine/Engine.h"

extern CORE_API map<FString, UClass*> ClassMap;
shared_ptr<UEngine> GEngine;

int32 FEngineLoop::PreInit(const TCHAR* CmdLine)
{
	UEngine* Test;
	return 0;
}

int32 FEngineLoop::Init()
{
	GEngine = NewObject<UEngine>(nullptr);
	return 0;
}

void FEngineLoop::Tick()
{
}
