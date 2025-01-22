#include "LaunchEngineLoop.h"
#include "Engine/Engine.h"

CORE_API map<FString, UClass*>& GetClassMap(); 

FEngineLoop::~FEngineLoop()
{
	FConfigCacheIni::Get(true);
	FModuleManager::Get(true);
	FLogger::Get(true);


	// Unregister Class
	{
		for (auto It : GetClassMap())
		{
			GetObjectArray().Free(typeid(UClass), It.second);
			It.second = nullptr;
		}
		GetClassMap().clear();
	}
}

int32 FEngineLoop::PreInit(const TCHAR* CmdLine)
{
	FLogger::Get();
	FModuleManager::Get();
	FConfigCacheIni::Get();

#if WITH_EDITOR
#ifdef USER_PROJECT_NAME
	{
		const FString UserProjectName = TEXT(USER_PROJECT_NAME);
		if (!UserProjectName.empty())
		{
			FModuleManager::Get()->LoadModule(FName(UserProjectName));
		}
	}
#endif
#endif

	// CDO 객체를 생성한다
	for (auto It : GetClassMap()) 
	{
		It.second->GetDefaultObject();
	}
	return 0;
}

int32 FEngineLoop::Init(HWND hViewportWnd)
{
	GEngine = NewObject<UEngine>(nullptr);
	GEngine->Init(hViewportWnd);
	return 0;
}

void FEngineLoop::Tick()
{
}
