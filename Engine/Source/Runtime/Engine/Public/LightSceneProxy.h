#pragma once

#include "GameFramework/Actor.h"

class ULightComponent;
struct FLightShaderParameters;

/**
 * 렌더링 스레드에서 조명을 렌더링하는 데 사용되는 데이터를 캡슐화합니다.
 * 생성자는 게임 스레드에서 호출되며, 그 후에는 렌더링 스레드가 객체를 소유합니다.
 * FLightSceneProxy는 엔진 모듈에 있으며 다양한 유형의 조명을 구현하기 위해 서브클래스화됩니다.
 */
class ENGINE_API FLightSceneProxy
{
public:
    /** Initialization constructor. */
    FLightSceneProxy(/*const*/ ULightComponent* InLightComponent);
    virtual ~FLightSceneProxy();

protected:

    friend class FScene;

    /** 빛 구성 요소입니다. */
    /*const*/ ULightComponent* LightComponent;
};