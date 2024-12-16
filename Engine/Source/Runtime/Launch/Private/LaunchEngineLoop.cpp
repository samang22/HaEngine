#include "LaunchEngineLoop.h"
#include "Engine/Engine.h"

extern CORE_API map<FString, UClass*> ClassMap;

int32 FEngineLoop::PreInit(const TCHAR* CmdLine)
{
	UEngine* Test;
	return 0;
}

int32 FEngineLoop::Init()
{
	UEngine* Test = nullptr;
	UClass* Temp = UEngine::StaticClass();
	UClass* EngineClass = ClassMap[TEXT("UEngine")];
	shared_ptr<UEngine> Engine = Cast<UEngine>(NewObject<UObject>(nullptr, EngineClass));
	return 0;
}

void FEngineLoop::Tick()
{
}
