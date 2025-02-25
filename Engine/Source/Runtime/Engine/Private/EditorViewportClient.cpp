#include "EditorViewportClient.h"

void UEditorViewportClient::Init(HWND hInViewportHandle, UWorld* InWorld)
{
    Super::Init(hInViewportHandle, InWorld);
    LastMouseState = DirectX::Mouse::Get().GetState();
}

void UEditorViewportClient::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    UpdateCameraMovement(DeltaTime);
    UpdateMouseDelta();
}

void UEditorViewportClient::Draw()
{
    Super::Draw();
}

void UEditorViewportClient::UpdateCameraMovement(float DeltaTime)
{
    DirectX::Mouse::State CurrentMouseState = DirectX::Mouse::Get().GetState();
    if (!CurrentMouseState.rightButton)
    {
        return;
    }

    DirectX::Keyboard::State KeyboardState = DirectX::Keyboard::Get().GetState();

    bool bForwardKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::W);
    bool bBackwardKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::S);
    bool bRightKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::D);
    bool bLeftKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::A);

    if (bForwardKeyState)
    {
        E_LOG(Warning, TEXT("Forward"));
    }
    if (bBackwardKeyState)
    {
        E_LOG(Warning, TEXT("bBackwardKeyState"));
    }
}

void UEditorViewportClient::UpdateMouseDelta()
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

    DirectX::Mouse::State CurrentMouseState = DirectX::Mouse::Get().GetState();
    FUpdateLastMouseState UpdateLastMouseStateScope{ .LastMouseState = LastMouseState, .CurrentMouseState = CurrentMouseState };

    if (!CurrentMouseState.rightButton)
    {
        return;
    }

    //E_LOG(Log, TEXT("r btn clicked"));
}