#pragma once
#include "CoreMinimal.h"
#include "RHI.h"

/**
 * 씬의 private scene manager 구현에 대한 인터페이스입니다. GetRendererModule().AllocateScene을 사용하여 생성하세요.
 */
class FSceneInterface
{
    friend class FRendererModule;
public:
    ENGINE_API FSceneInterface(ERHIFeatureLevel::Type InFeatureLevel);

    virtual void Release() {}

protected:
    virtual ~FSceneInterface() {}

    /** 이 씬의 기능 수준 */
    ERHIFeatureLevel::Type FeatureLevel;
};