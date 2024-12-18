#include "LaunchEngineLoop.h"
#include "Engine/Engine.h"

CORE_API map<FString, UClass*>& GetClassMap(); 
shared_ptr<UEngine> GEngine;

FEngineLoop::~FEngineLoop()
{
	FLogger::Get(true);


	// Unregister Class
	{
		for (auto It : GetClassMap())
		{
			GUObjectArray.Free(typeid(UClass), It.second);
			It.second = nullptr;
		}
		GetClassMap().clear();
	}
}

int32 FEngineLoop::PreInit(const TCHAR* CmdLine)
{
	FLogger::Get();

	// CDO 객체를 생성한다
	for (auto It : GetClassMap()) 
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
