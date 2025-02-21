#include "Engine/Engine.h"
#include "EditorViewportClient.h"

IMPLEMENT_MODULE(FDefaultModuleImpl, Engine);

TObjectPtr<UEngine> GEngine;
ENGINE_API UWorld* GWorld = nullptr;

UEngine::UEngine()
{
	if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { return; }
	E_LOG(Warning, TEXT("{} Hello"), GetName());
}

void UEngine::Init(HWND hViewportHandle)
{
	MainViewportHandle = hViewportHandle;

	World = NewObject<UWorld>(this, UWorld::StaticClass(), TEXT("Editor World"));
	World->WorldType - EWorldType::Editor;
	EditorWorld = World;
	GWorld = World.get();

	GWorld->InitalizeNewWorld();

	EditorViewportClient = NewObject<UEditorViewportClient>(this, nullptr, TEXT("EditorViewportClient"));
	EditorViewportClient->Init(hViewportHandle, GWorld);
    CurrentViewportClient = Cast<UViewportClient>(EditorViewportClient);
}

void UEngine::Tick(float DeltaSeconds)
{
    GWorld->Tick(DeltaSeconds);
	EditorViewportClient->Draw();
}
  
void UEngine::PreExit()
{
	EditorViewportClient = nullptr;
	CurrentViewportClient = nullptr;
}

string UEngine::Save()
{
    return World->Save();
}

void UEngine::Load(const string& InLoadString)
{
    World->Load(InLoadString);
}

void UEngine::WndProc(UINT Message, WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{
    switch (Message)
    {
    case WM_SIZE:
    {
        const bool bWasMinized = (wParam == SIZE_MINIMIZED);
        if (wParam == SIZE_RESTORED || wParam == SIZE_MAXIMIZED)
        {
            const int32 NewWidth = (int)(short)(LOWORD(lParam));
            const int32 NewHeight = (int)(short)(HIWORD(lParam));
            if (CurrentViewportClient)
            {
                CurrentViewportClient->RequestResize(NewWidth, NewHeight);
            }
        }
        break;
    }
    default:
        break;
    }
}
