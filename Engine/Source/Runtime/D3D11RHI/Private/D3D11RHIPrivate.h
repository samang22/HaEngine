#pragma once
/*=============================================================================
    D3D11RHIPrivate.h: Private D3D RHI 정의.
=============================================================================*/

#include "DynamicRHI.h"
#include "D3D11Util.h"
#include "D3D11ThirdParty.h"

// 쉽게 생각해서 그래픽 카드에 대응되는 Struct라고 보면 됩니다
struct FD3D11Adapter
{
    /** 지원되지 않거나 FindAdapter()가 호출되지 않은 경우 null입니다. */
    TRefCountPtr<IDXGIAdapter> DXGIAdapter;

    DXGI_ADAPTER_DESC DXGIAdapterDesc;

    /** 지원되는 최대 D3D11 기능 수준입니다. 지원되지 않거나 FindAdapter()가 호출되지 않은 경우 0입니다. */    
    D3D_FEATURE_LEVEL MaxSupportedFeatureLevel;

    /** Whether this is a software adapter */
    bool bSoftwareAdapter;

    /** GPU가 통합형인지 또는 개별형인지 여부를 나타냅니다. */
    bool bIsIntegrated;

    FD3D11Adapter()
    {
    }

    FD3D11Adapter(TRefCountPtr<IDXGIAdapter> InDXGIAdapter, D3D_FEATURE_LEVEL InMaxSupportedFeatureLevel, bool bInSoftwareAdatper, bool InIsIntegrated)
        : DXGIAdapter(InDXGIAdapter)
        , MaxSupportedFeatureLevel(InMaxSupportedFeatureLevel)
        , bSoftwareAdapter(bInSoftwareAdatper)
        , bIsIntegrated(InIsIntegrated)
    {
        if (DXGIAdapter.IsValid())
        {
            VERIFYD3D11RESULT(DXGIAdapter->GetDesc(&DXGIAdapterDesc));
        }
    }


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

/** 동적으로 바인딩된 RHI가 구현하는 인터페이스입니다. */
class D3D11RHI_API FD3D11DynamicRHI : public FDynamicRHI
{

};