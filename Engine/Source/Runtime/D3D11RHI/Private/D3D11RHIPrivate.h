#pragma once
/*=============================================================================
    D3D11RHIPrivate.h: Private D3D RHI 정의.
=============================================================================*/

#include "DynamicRHI.h"
#include "D3D11Util.h"
#include "D3D11ThirdParty.h"
#include "D3D11StateCache.h"
#include "RHIContext.h"

typedef ID3D11DeviceContext FD3D11DeviceContext;
typedef ID3D11Device FD3D11Device;

// 쉽게 생각해서 그래픽 카드에 대응되는 Struct라고 보면 된다.
struct FD3D11Adapter
{
    /** 지원되지 않거나 FindAdapter()가 호출되지 않은 경우 null입니다. */
    TRefCountPtr<IDXGIAdapter> DXGIAdapter;

    DXGI_ADAPTER_DESC DXGIAdapterDesc = {};

    /** 지원되는 최대 D3D11 기능 수준입니다. 지원되지 않거나 FindAdapter()가 호출되지 않은 경우 0입니다. */    
    D3D_FEATURE_LEVEL MaxSupportedFeatureLevel = {};

    /** Whether this is a software adapter */
    bool bSoftwareAdapter = false;

    /** GPU가 통합형인지 또는 개별형인지 여부를 나타냅니다. */
    bool bIsIntegrated = false;

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
class D3D11RHI_API FD3D11DynamicRHI : public FDynamicRHI, public IRHICommandContext
{
public:

    // Accessors.
    ID3D11Device* GetDevice() const
    {
        return Direct3DDevice;
    }
    FD3D11DeviceContext* GetDeviceContext() const
    {
        return Direct3DDeviceIMContext;
    }

public:
    virtual FViewportRHIRef RHICreateViewport(void* WindowHandle, uint32 SizeX, uint32 SizeY, bool bIsFullscreen, EPixelFormat PreferredPixelFormat) override;
    virtual class IRHICommandContext* RHIGetDefaultContext() final override;

public:
    /** Initialization constructor. */
    FD3D11DynamicRHI(IDXGIFactory1* InDXGIFactory1, D3D_FEATURE_LEVEL InFeatureLevel, const FD3D11Adapter& InAdapter);

    /** 아직 초기화되지 않은 경우, D3D 장치를 초기화합니다. */
    virtual void InitD3DDevice();

    // FDynamicRHI interface.
    virtual void Init() override;
    virtual void Shutdown() override;

public:
    void ClearState();

protected:
    /** The global D3D interface. */
    TRefCountPtr<IDXGIFactory1> DXGIFactory1;

    /** The feature level of the device. */
    D3D_FEATURE_LEVEL FeatureLevel;

    /** The global D3D device's immediate context */
    TRefCountPtr<FD3D11Device> Direct3DDevice;

    /** The global D3D device's immediate context */
    TRefCountPtr<FD3D11DeviceContext> Direct3DDeviceIMContext;

    HANDLE ExceptionHandlerHandle = INVALID_HANDLE_VALUE;

protected:
    FD3D11Adapter Adapter;

    FD3D11StateCache StateCache;
};