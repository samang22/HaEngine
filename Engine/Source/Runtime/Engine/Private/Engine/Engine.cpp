#include "Engine/Engine.h"
#include "EditorViewportClient.h"
#include "EngineModule.h"


IMPLEMENT_MODULE(FEngineModule, Engine);

void FEngineModule::StartupModule()
{
    FDefaultModuleImpl::StartupModule();

    Keyboard = make_unique<DirectX::Keyboard>();
    Mouse = make_unique<DirectX::Mouse>();
}

void FEngineModule::ShutdownModule()
{
    FDefaultModuleImpl::ShutdownModule();
}

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
    EditorViewportClient->Tick(DeltaSeconds);
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
    case WM_ACTIVATE:
    case WM_ACTIVATEAPP:
        DirectX::Keyboard::ProcessMessage(Message, wParam, lParam);
        break;
    case WM_SYSKEYDOWN:
        if (wParam == VK_RETURN && (lParam & 0x60000000) == 0x20000000) {
            // ALT+ENTER를 사용한 전체 화면 전환 처리 등
        }
        DirectX::Keyboard::ProcessMessage(Message, wParam, lParam);
        break;
    case WM_KEYDOWN:
    case WM_KEYUP:
    case WM_SYSKEYUP:
        DirectX::Keyboard::ProcessMessage(Message, wParam, lParam);
        break;
    case WM_MOUSEMOVE:
    case WM_LBUTTONDOWN:
    case WM_LBUTTONUP:
    case WM_RBUTTONDOWN:
    case WM_RBUTTONUP:
    case WM_MBUTTONDOWN:
    case WM_MBUTTONUP:
    case WM_MOUSEWHEEL:
    case WM_XBUTTONDOWN:
    case WM_XBUTTONUP:
    case WM_MOUSEHOVER:
        DirectX::Mouse::ProcessMessage(Message, wParam, lParam);
        break;
    default:
        break;
    }
}
