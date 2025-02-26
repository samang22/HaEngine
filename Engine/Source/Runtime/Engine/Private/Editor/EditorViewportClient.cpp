#include "Editor/EditorViewportClient.h"
#include "Editor/CameraController.h"
#include "RenderResource.h"
#include "RendererInterface.h"
#include "SceneView.h"
#include "EngineModule.h"
#include "Engine/World.h"

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
    //Super::Draw();

    if (!GetScene()) { return; }
    // 뷰포트를 위한 FSceneViewFamily/FSceneView 설정
    FSceneViewFamilyContext ViewFamily(FSceneViewFamily::ConstructionValues(Viewport, GetScene()));

    FRotator ViewRotation = GetViewRotation();

    ViewFamily.ViewRotationMatrix = FInverseRotationMatrix(ViewRotation);

    // 언리얼 축에 맞게 View Matrix를 회전하고 있습니다.
    // X가 전후방
    // Y가 좌우
    // Z가 상하
    ViewFamily.ViewRotationMatrix = ViewFamily.ViewRotationMatrix * FMatrix(
        FPlane(0, 0, 1, 0),
        FPlane(1, 0, 0, 0),
        FPlane(0, 1, 0, 0),
        FPlane(0, 0, 0, 1));

    FVector ViewOrigin = GetViewLocation();
    FMatrix ViewTranslationMatrix = FMatrix::CreateTranslation(-ViewOrigin);

    ViewFamily.ViewMatrix = ViewTranslationMatrix * ViewFamily.ViewRotationMatrix;
    ViewFamily.ProjectionMatrix = FMatrix::CreatePerspectiveFieldOfView(3.14f / 4.f, 16.f / 9.f, 100.f, 1000.f);
    GetRendererModule().BeginRenderingViewFamily(&ViewFamily);
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

    // 위치/회전이 변경되었는지 확인
    const bool bTransformDifferent = !NewViewLocation.Equals(GetViewLocation(), UE_SMALL_NUMBER) || NewViewRotation != GetViewRotation();
    if (bTransformDifferent)
    {
        MoveViewportPerspectiveCamera(
            NewViewLocation - GetViewLocation(),
            NewViewRotation - GetViewRotation());
    }
}

void UEditorViewportClient::MoveViewportPerspectiveCamera(const FVector& InDrag, const FRotator& InRot)
{
    FVector ViewLocation = GetViewLocation();
    FRotator ViewRotation = GetViewRotation();

    {
        // 카메라 회전 업데이트
        ViewRotation += FRotator(InRot.Pitch, InRot.Yaw, InRot.Roll);

        // -180도에서 180도로 정규화
        ViewRotation.Pitch = FRotator::NormalizeAxis(ViewRotation.Pitch);
        // 나중에 카메라 방향 변환에서 발생할 수 있는 수치적 문제를 피하기 위해 피치를 ±90도 (작은 허용 오차를 포함하여) 이내로 유지
        ViewRotation.Pitch = FMath::Clamp(ViewRotation.Pitch, -90.f + UE_SMALL_NUMBER, 90.f - UE_SMALL_NUMBER);
    }

    // Update camera Location
    ViewLocation += InDrag;

    SetViewLocation(ViewLocation);
    SetViewRotation(ViewRotation);
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