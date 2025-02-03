#pragma once
#include "CoreMinimal.h"
#include "RHIFeatureLevel.h"

/** 동적으로 바인딩된 RHI가 구현하는 인터페이스입니다. */
class FDynamicRHI
{
public:
};


/** 동적 RHI(랜더링 하드웨어 인터페이스)를 구현하는 모듈의 인터페이스를 정의합니다. */
class IDynamicRHIModule : public IModuleInterface
{
public:

    /** Checks whether the RHI is supported by the current system. */
    virtual bool IsSupported() = 0;

    virtual bool IsSupported(ERHIFeatureLevel::Type RequestedFeatureLevel) { return IsSupported(); }

    /** Creates a new instance of the dynamic RHI implemented by the module. */
    virtual FDynamicRHI* CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel = ERHIFeatureLevel::Num) = 0;
};

/**
*   동적 RHI를 사용하는 각 플랫폼은 이 함수를 구현해야 합니다.
*   동적 RHI 인스턴스를 생성하기 위해 호출됩니다.
*/
FDynamicRHI* PlatformCreateDynamicRHI();