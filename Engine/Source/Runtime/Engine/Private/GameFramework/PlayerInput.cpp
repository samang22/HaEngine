#include "GameFramework/PlayerInput.h"
#include "GameFramework/PlayerController.h"
#include "Components/InputComponent.h"
#include "Engine/Engine.h"
#include "Engine/GameViewportClient.h"

TArray<FInputAxisKeyMapping> UPlayerInput::EngineDefinedAxisMappings;

UPlayerInput::UPlayerInput()
{
    if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { return; }

    SetLockMouseMode(bLockMouse);
    LastMouseState = InitialMouseState;
}

UPlayerInput::~UPlayerInput()
{
    if (HasAnyFlags(EObjectFlags::RF_ClassDefaultObject)) { return; }
    while (ShowCursor(TRUE) < 0);
}

void UPlayerInput::SetLockMouseMode(bool bMode)
{
#if SERVER
    bLockMouse = false;
    return;
#endif

    InitialMouseState = DirectX::Mouse::Get().GetState();
    bLockMouse = bMode;
    if (bLockMouse)
    {
        HWND hViewportHandle = GEngine->GetCurrentViewClient()->GetNativeWindowHandle();
        RECT WndRect = {};
        GetClientRect(hViewportHandle, &WndRect);
        int CenterX = (WndRect.right + WndRect.left) / 2;
        int CenterY = (WndRect.bottom + WndRect.top) / 2;
        InitialMouseState.x = CenterX;
        InitialMouseState.y = CenterY;

        POINT Point = POINT(InitialMouseState.x, InitialMouseState.y);
        ClientToScreen(hViewportHandle, &Point);
        SetCursorPos(Point.x, Point.y); // 변환된 화면 좌표를 사용합니다

        LPARAM lParam = MAKELPARAM(InitialMouseState.x, InitialMouseState.y); // 변환된 화면 좌표를 사용합니다
        DirectX::Mouse::Get().ProcessMessage(WM_MOUSEMOVE, 0, lParam);

        ShowCursor(FALSE);
    }
    else
    {
        ShowCursor(TRUE);
    }
}

void UPlayerInput::ProcessInputStack(const TArray<UInputComponent*>& InputComponentStack, const float DeltaTime, const bool bGamePaused)
{
    APlayerController* PlayerController = GetOuterAPlayerController();
    if (PlayerController)
    {
        //PlayerController->PreProcessInput(DeltaTime, bGamePaused);
    }

    // EvaluateInputDelegates
    {
        struct FUpdateLastMouseState
        {
            ~FUpdateLastMouseState()
            {
                LastMouseState = CurrentMouseState;
            }
            DirectX::Mouse::State& LastMouseState;
            DirectX::Mouse::State& CurrentMouseState;
        };

        DirectX::Keyboard::State KeyboardState = DirectX::Keyboard::Get().GetState();

        DirectX::Mouse::State MouseState = DirectX::Mouse::Get().GetState();
        FUpdateLastMouseState UpdateLastMouseStateScope{ .LastMouseState = LastMouseState, .CurrentMouseState = MouseState };

        FVector2D Delta = FVector2D(MouseState.x - LastMouseState.x, LastMouseState.y - MouseState.y);

        if (bLockMouse)
        {
            HWND hViewportHandle = GEngine->GetCurrentViewClient()->GetNativeWindowHandle();
            // 우리 창(최상위 부모 기준)이 포그라운드일 때만 마우스를 고정한다
            const bool bForeground = (GetForegroundWindow() == GetAncestor(hViewportHandle, GA_ROOT));

            if (bForeground)
            {
                if (bWasForeground)
                {
                    Delta = FVector2D(MouseState.x - InitialMouseState.x, InitialMouseState.y - MouseState.y);
                }
                else
                {
                    // 포커스 복귀 첫 프레임: 커서가 임의 위치에 있으므로 델타를 무시하고 커서를 다시 숨긴다
                    Delta = FVector2D(0.f, 0.f);
                    while (ShowCursor(FALSE) >= 0);
                }

                POINT Point = POINT(InitialMouseState.x, InitialMouseState.y);
                // 우리 윈도우의 상대 좌표를 Windows의 Screen 좌표로 변환한다
                ClientToScreen(hViewportHandle, &Point);
                SetCursorPos(Point.x, Point.y);
            }
            else
            {
                // 포커스를 잃은 동안에는 마우스를 자유롭게 두고 카메라 입력도 차단한다
                Delta = FVector2D(0.f, 0.f);
                if (bWasForeground)
                {
                    while (ShowCursor(TRUE) < 0);
                }
            }

            bWasForeground = bForeground;
        }

        for (UInputComponent* const InputComponent : InputComponentStack)
        {
            if (!InputComponent->AxisBindings.empty())
            {
                for (FInputAxisBinding& ItAxisBinding : InputComponent->AxisBindings)
                {
                    // 동일한 이름의 AxisKeyMapping 수집
                    TArray<FInputAxisKeyMapping> Results;
                    Results.reserve(EngineDefinedAxisMappings.size());
                    std::for_each(EngineDefinedAxisMappings.begin(), EngineDefinedAxisMappings.end(),
                        [&ItAxisBinding, &Results](FInputAxisKeyMapping& Mapping)
                        {
                            if (ItAxisBinding.AxisName == Mapping.AxisName)
                            {
                                Results.push_back(Mapping);
                            }
                        });

                    float Scale = 0.f;
                    for (auto& ItFind : Results)
                    {
                        if (ItFind.Key == EKeys::MouseX || ItFind.Key == EKeys::MouseY)
                        {
                            Scale = ItFind.Key == EKeys::MouseX ? Scale += Delta.x : Scale += Delta.y;
                            Scale *= ItFind.Scale;
                        }
                        else
                        {
                            const bool bKeyDown = KeyboardState.IsKeyDown(ItFind.Key);
                            if (bKeyDown)
                            {
                                Scale += ItFind.Scale;
                            }
                        }
                    }
                    ItAxisBinding.AxisDelegate.Broadcast(Scale);
                }
            }
        }
    }

    if (PlayerController)
    {
        //PlayerController->PostProcessInput(DeltaTime, bGamePaused);
    }
}

APlayerController* UPlayerInput::GetOuterAPlayerController() const
{
    return dynamic_cast<APlayerController*>(GetOuter());
}

void UPlayerInput::AddEngineDefinedAxisMapping(const FInputAxisKeyMapping& AxisMapping)
{
    auto It = find_if(EngineDefinedAxisMappings.begin(), EngineDefinedAxisMappings.end(),
        [&](const FInputAxisKeyMapping& Mapping)
        {
            return Mapping == AxisMapping;
        });
    if (It == EngineDefinedAxisMappings.end())
    {
        EngineDefinedAxisMappings.push_back(AxisMapping);
    }
    else
    {
        E_LOG(Error, TEXT("중복 FInputAxisKeyMapping 입니다."));
    }
}