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

void UEngine::UpdateTimeAndHandleMaxTickRate()
{
    // 이것은 항상 실시간으로 처리되며 고정 프레임 속도로 조정되지 않습니다. 현재 실시간보다 약간 빨리 시작합니다.
    static double LastRealTime = FPlatformTime::Seconds() - 0.0001;

    // 논리적 마지막 시간을 지난 틱의 논리적 현재 시간과 일치하도록 업데이트합니다.
    FApp::UpdateLastTime();

    {
        // 논리적 시간을 현재 시간으로 업데이트합니다. // [우리는 해당 x]이는 아래의 고정 프레임 속도에 의해 변경될 수 있습니다.
        double CurrentRealTime = FPlatformTime::Seconds();
        FApp::SetCurrentTime(CurrentRealTime);

        // 델타 시간을 계산합니다. 이는 실시간 초 단위입니다.
        double DeltaRealTime = CurrentRealTime - LastRealTime;

        // 델타 타임이 너무 낮은 경우 또는 높은 경우 보정을 한다
        const double MaxTickRate = GetMaxTickRate(DeltaRealTime);

        double WaitTime = 0.f;
        // 최대 FPS에서 대기 시간으로 변환합니다.
        if (MaxTickRate > 0)
        {
            WaitTime = FMath::Max(1.0 / MaxTickRate - DeltaRealTime, 0.0);
        }

        // 최대 프레임 속도와 부드러운 프레임 속도를 적용하기 위해 대기합니다.
        if (WaitTime > 0.f)
        {
            double WaitEndTime = CurrentRealTime + WaitTime;

            // 5ms 이상 기다려야 하는 경우 슬립합니다. 우리는 PC에서 시작할 때 스케줄러의 세분도를 1ms로 설정합니다.
            // 2ms의 여유 시간을 예약하고, 이를 대기하기 위해 타임슬라이스를 포기합니다.
            if (WaitTime > 5.f / 1000.f)
            {
                uint32 Milliseconds = (uint32)((WaitTime - 0.005f) * 1000.0);
            }

            while (FPlatformTime::Seconds() < WaitEndTime)
            {
                ::SwitchToThread();
            }
        }

        CurrentRealTime = FPlatformTime::Seconds();
        FApp::SetCurrentTime(CurrentRealTime);
        // 논리적 현재 시간을 기준으로 논리적 델타 시간을 업데이트합니다.
        FApp::SetDeltaTime(FApp::GetCurrentTime() - LastRealTime);

        LastRealTime = CurrentRealTime;

        // 원하는 경우 최대 델타 시간을 적용합니다.
        // 한 프레임이 튀어서 텔타 타임이 너무 큰 경우(Ex. 5초가 지남) 강제로 줄입니다
        const float MaxDeltaTime = 1.f / 5.f;
        // 호스트 또는 클라이언트로서 네트워크 클라이언트를 다루는 경우 델타 시간을 수정하고 싶지 않습니다.
        //if (World != NULL
        //    // 게임 정보가 없는 것은 클라이언트를 의미합니다.
        //    && ((World->GetAuthGameMode() != NULL
        //        // NumPlayers와 GamePlayer는 스탠드얼론 게임 유형에서만 일치하며, 스플릿스크린의 경우를 처리합니다.
        //        && World->GetAuthGameMode()->GetNumPlayers() == NumGamePlayers)))
        if (MaxDeltaTime > 0.f)
        {
            FApp::SetDeltaTime(FMath::Min<double>(FApp::GetDeltaTime(), MaxDeltaTime));
        }

        // FPS 계산
        {
            static float ElapsedTime = 0.f;
            static int FrameCount = 0;
            ElapsedTime += FApp::GetDeltaTime();
            FrameCount++;
            if (ElapsedTime > 1.f)
            {
                const float FPS = FrameCount / ElapsedTime;
                FApp::SetFPS(FPS);
                //E_LOG(Warning, TEXT("FPS: {}"), FPS);

                ElapsedTime -= 1.f;
                FrameCount = 0;
            }
        }
    }
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

double UEngine::GetMaxTickRate(double DeltaTime)
{
    double MaxTickRate = 0.0;
    MaxTickRate = 1.0 / DeltaTime;
    MaxTickRate = FMath::Min(MaxTickRate, 120.0);
    return MaxTickRate;
}
