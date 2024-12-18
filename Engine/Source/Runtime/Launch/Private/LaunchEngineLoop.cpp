#include "LaunchEngineLoop.h"
#include "Engine/Engine.h"

extern CORE_API map<FString, UClass*> ClassMap;
shared_ptr<UEngine> GEngine;

FEngineLoop::~FEngineLoop()
{
	FLogger::Get(true);
}

int32 FEngineLoop::PreInit(const TCHAR* CmdLine)
{
	FLogger::Get();

	// CDO 객체를 생성한다
	for (auto It : ClassMap)
	{
		It.second->GetDefaultObject();
	}
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
