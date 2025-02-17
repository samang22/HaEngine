#include "LaunchEngineLoop.h"
#include "Engine/Engine.h"
#include "RHI.h"                                                                            
#include "ShaderCompiler.h"

CORE_API map<FString, UClass*>& GetClassMap(); 

FEngineLoop::~FEngineLoop()
{
	FModuleManager::Get(true);
}

int32 FEngineLoop::PreInit(const TCHAR* CmdLine)
{
	FLogger::Get();
	FModuleManager::Get();
	FConfigCacheIni::Get();

	{
		RHIInit();
	}

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

	{
		// Shader를 컴파일 한다
		FShaderCompilingManager::Get();
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
	GEngine->Tick(0.f);
}

void FEngineLoop::Exit()
{
	GEngine->PreExit();
	RHIExit();

	FConfigCacheIni::Get(true);

	GEngine = nullptr;

	// Unregister Class
	{
		for (auto It : GetClassMap())
		{
			It.second->~UClass();
			GetObjectArray().Free(typeid(UClass), It.second);
			It.second = nullptr;
		}
		GetClassMap().clear();
	}

	{
		GetObjectArray().Destroy();
	}

	FLogger::Get(true);
}

void FEngineLoop::WndProc(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
	if (GEngine)
	{
		GEngine->WndProc(message, wParam, lParam, pResult);
	}
}