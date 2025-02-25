#pragma once
#include "ViewportClient.h"
#include "EditorViewportClient.generated.h"

// Unreal 에서는 FViewport 에서 다양한 ViewportClient(Ex. UGameViewportClient, FEditorViewportClient 등)를 대상으로
// 입력 정보와 렌더링 요청을 전달하고 있는 것으로 보임.
// 이를 간소화 하여 UViewportClient를 만들었고, 여기에 직접적으로 명령을 전달할 예정입니다.
UCLASS()
class ENGINE_API UEditorViewportClient : public UViewportClient
{
    GENERATED_BODY()
public:
    virtual void Init(HWND hInViewportHandle, UWorld* InWorld) override;
    virtual void Tick(float DeltaTime) override;
    virtual void Draw() override;

protected:
    /**
     * 실시간 카메라 움직임을 업데이트합니다. 뷰포트의 각 틱마다 호출되어야 합니다!
     *
     * @param DeltaTime 마지막 업데이트 이후 경과된 시간(초)
     */
    void UpdateCameraMovement(float DeltaTime);

    /**
     * 델타 마우스 움직임을 기반으로 뷰포트를 업데이트하기 위해 매 프레임마다 호출됩니다.
     */
    virtual void UpdateMouseDelta();

private:
    DirectX::Mouse::State LastMouseState;
};