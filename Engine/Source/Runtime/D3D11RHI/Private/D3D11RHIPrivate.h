#pragma once
/*=============================================================================
    D3D11RHIPrivate.h: Private D3D RHI 정의.
=============================================================================*/

#include "DynamicRHI.h"
#include "D3D11ThirdParty.h"

// 쉽게 생각해서 그래픽 카드에 대응되는 Struct라고 보면 됩니다
struct FD3D11Adapter
{
    /** 지원되지 않거나 FindAdapter()가 호출되지 않은 경우 null입니다. */
    TRefCountPtr<IDXGIAdapter> DXGIAdapter;

    /** The maximum D3D11 feature level supported. 0 if not supported or FindAdapter() wasn't called */
    D3D_FEATURE_LEVEL MaxSupportedFeatureLevel;

    bool IsValid() const
    {
        return DXGIAdapter.IsValid();
    }
};

/** D3D11RHI 모듈을 동적 RHI 제공 모듈로 구현합니다. */
class FD3D11DynamicRHIModule : public IDynamicRHIModule
{
public:
    virtual void StartupModule() override;

    // IDynamicRHIModule
    virtual bool IsSupported() override;
    virtual FDynamicRHI* CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel = ERHIFeatureLevel::Num) override;

private:
    FD3D11Adapter ChosenAdapter;

    // MaxSupportedFeatureLevel과 ChosenAdapter 설정
    void FindAdapter();
};