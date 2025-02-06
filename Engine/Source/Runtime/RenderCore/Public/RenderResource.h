#pragma once
#include "CoreTypes.h"
#include "RHIDefinitions.h"
#include "RHIResources.h"
#include "PixelFormat.h"
#include "RHI.h"
#include "RHICommandList.h"

/**
 * 렌더링 스레드가 소유하는 렌더링 리소스입니다.
 */
class FRenderResource
{
public:
    ////////////////////////////////////////////////////////////////////////////////////

    /**
     * 렌더링 리소스의 초기화 순서를 제어합니다.
     * 초기 엔진 리소스는 정적 초기화 순서 문제를 피하기 위해 'Pre' 단계를 사용합니다.
     */
    enum class EInitPhase : uint8
    {
        Pre,
        Default,
        MAX
    };

    /** 현재 초기화된 모든 렌더링 리소스를 해제합니다. */
    static RENDERCORE_API void ReleaseRHIForAllResources();

    /** RHI가 초기화되기 전에 초기화된 모든 리소스를 초기화합니다. */
    static RENDERCORE_API void InitPreRHIResources();

};