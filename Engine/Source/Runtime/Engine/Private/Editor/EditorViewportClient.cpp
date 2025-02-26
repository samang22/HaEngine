#include "Editor/EditorViewportClient.h"
#include "Editor/CameraController.h"

UEditorViewportClient::~UEditorViewportClient()
{
    if (CameraUserImpulseData)
    {
        delete CameraUserImpulseData;
        CameraUserImpulseData = nullptr;
    }

    if (CameraController)
    {
        delete CameraController;
        CameraController = nullptr;
    }
}

void UEditorViewportClient::Init(HWND hInViewportHandle, UWorld* InWorld)
{
    Super::Init(hInViewportHandle, InWorld);
    LastMouseState = DirectX::Mouse::Get().GetState();

    CameraUserImpulseData = new FCameraControllerUserImpulseData();
    CameraController = new FEditorCameraController();
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
    DirectX::Keyboard::State KeyboardState = DirectX::Keyboard::Get().GetState();

    // 확대 제스처에서 임펄스를 적용하고, WASD 키를 사용하는 경우 임펄스를 초기화합니다.
    CameraUserImpulseData->MoveForwardBackwardImpulse = 0.0f;// GestureMoveForwardBackwardImpulse;
    CameraUserImpulseData->MoveRightLeftImpulse = 0.0f;
    CameraUserImpulseData->MoveUpDownImpulse = 0.0f;
    CameraUserImpulseData->RotateYawImpulse = 0.0f;
    CameraUserImpulseData->RotatePitchImpulse = 0.0f;
    CameraUserImpulseData->RotateRollImpulse = 0.0f;

    DirectX::Mouse::State CurrentMouseState = DirectX::Mouse::Get().GetState();
    bool bRightBtnKeyState = CurrentMouseState.rightButton;
    bool bForwardKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::W);
    bool bBackwardKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::S);
    bool bRightKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::D);
    bool bLeftKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::A);
    bool bUpKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::Q);
    bool bDownKeyState = KeyboardState.IsKeyDown(DirectX::Keyboard::E);
    
    if (bRightBtnKeyState)
    {
        if (bForwardKeyState)
        {
            CameraUserImpulseData->MoveForwardBackwardImpulse += 1.f;
        }
        if (bBackwardKeyState)
        {
            CameraUserImpulseData->MoveForwardBackwardImpulse -= 1.f;
        }

        if (bRightKeyState)
        {
            CameraUserImpulseData->MoveRightLeftImpulse += 1.f;
        }

        if (bLeftKeyState)
        {
            CameraUserImpulseData->MoveRightLeftImpulse -= 1.f;
        }

        if (bUpKeyState)
        {
            CameraUserImpulseData->MoveUpDownImpulse += 1.f;
        }

        if (bDownKeyState)
        {
            CameraUserImpulseData->MoveUpDownImpulse -= 1.f;
        }
    }

    FVector NewViewLocation = GetViewLocation();
    FRotator NewViewRotation = GetViewRotation();

    // 카메라가 마우스나 키보드로 이동 중인지 확인합니다.
    bool bHasMovement = false;// GetDefault<ULevelEditorViewportSettings>()->bUseLegacyCameraMovementNotifications;
    const FCameraControllerUserImpulseData& ImpulseData = *CameraUserImpulseData;

    if (ImpulseData.RotateYawVelocityModifier != 0.0f ||
        ImpulseData.RotatePitchVelocityModifier != 0.0f ||
        ImpulseData.RotateRollVelocityModifier != 0.0f ||
        ImpulseData.MoveForwardBackwardImpulse != 0.0f ||
        ImpulseData.MoveRightLeftImpulse != 0.0f ||
        ImpulseData.MoveUpDownImpulse != 0.0f ||
        ImpulseData.RotateYawImpulse != 0.0f ||
        ImpulseData.RotatePitchImpulse != 0.0f ||
        ImpulseData.RotateRollImpulse != 0.0f)
    {
        bHasMovement = true;
    }

    const float FinalCameraSpeedScale = 20000.f;

    CameraController->UpdateSimulation(
        *CameraUserImpulseData,
        DeltaTime,
        FinalCameraSpeedScale,
        NewViewLocation, NewViewRotation);

    // 시뮬레이션을 업데이트한 후 회전 속도 수정자를 0으로 초기화합니다. 
    // 이러한 동작은 항상 순간적이기 때문입니다. 즉, 사용자가 마우스 움직임으로 
    // 몇 픽셀을 이동할 때 즉시 임펄스 값을 증가시킵니다.
    {
        CameraUserImpulseData->RotateYawVelocityModifier = 0.0f;
        CameraUserImpulseData->RotatePitchVelocityModifier = 0.0f;
        CameraUserImpulseData->RotateRollVelocityModifier = 0.0f;
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