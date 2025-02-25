#pragma once
#include "CoreMinimal.h"

/**
 * 장면 렌더링 함수들의 범위로 사용됩니다.
 * FSceneViewFamily::BeginRender에 의해 게임 스레드에서 초기화되고, 렌더링 스레드로 전달됩니다.
 * 렌더링 스레드는 Render()를 호출하고, 반환되면 장면 렌더러를 삭제합니다.
 */
class FSceneRenderer
{
public:
    FSceneRenderer(const FSceneViewFamily* InViewFamily/*, FHitProxyConsumer* HitProxyConsumer*/);
    virtual ~FSceneRenderer();

    virtual void Render();

    /** 렌더링 중인 뷰 패밀리. 이 변수는 Views 배열을 참조합니다. */
    FSceneViewFamily ViewFamily;

    struct FSceneUniformBuffer
    {
        FMatrix ViewMatrix = FMatrix::Identity;
        FMatrix ProjectionMatrix = FMatrix::Identity;
    };
    FSceneUniformBuffer SceneUniformBuffer;
    FUniformBufferRHIRef SceneUniformBufferRHI;
};