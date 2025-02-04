#include "D3D11RHIPrivate.h"


/**
 * CreateDXGIFactory1이 D3D11 DLL에서 지연 로드되는 가져오기이기 때문에, 사용자가 VistaSP2/DX10을 사용하지 않는 경우
 * CreateDXGIFactory1을 호출하면 예외가 발생합니다.
 * 우리는 SEH를 사용하여 그 경우를 감지하고 우아하게 실패 처리합니다.
 */
static void SafeCreateDXGIFactory(IDXGIFactory1** DXGIFactory1, bool bWithDebug)
{
    // CreateDXGIFactory2는 Win8.1+에서만 사용할 수 있으며, 존재하는 경우 찾습니다.
    uint32 Flags = bWithDebug ? DXGI_CREATE_FACTORY_DEBUG : 0;
    // 우리는 무조건 아래 기능을 지원한다고 보고 작업 한다.
    CreateDXGIFactory2(Flags, __uuidof(IDXGIFactory2), (void**)DXGIFactory1);
}

/**
 * 명령줄 매개변수에 기반하여 생성할 수 있는 가장 낮은 D3D 기능 수준을 반환합니다.
 */
static D3D_FEATURE_LEVEL GetMinAllowedD3DFeatureLevel()
{
    return D3D_FEATURE_LEVEL_11_0;
}

/**
 * 명령줄 매개변수에 기반하여 생성할 수 있는 가장 높은 D3D 기능 수준을 반환합니다.
 */
static D3D_FEATURE_LEVEL GetMaxAllowedD3DFeatureLevel()
{
    return D3D_FEATURE_LEVEL_11_1;
}

const TCHAR* GetFeatureLevelString(D3D_FEATURE_LEVEL FeatureLevel)
{
    switch (FeatureLevel)
    {
    case D3D_FEATURE_LEVEL_9_1:        return TEXT("9_1");
    case D3D_FEATURE_LEVEL_9_2:        return TEXT("9_2");
    case D3D_FEATURE_LEVEL_9_3:        return TEXT("9_3");
    case D3D_FEATURE_LEVEL_10_0:    return TEXT("10_0");
    case D3D_FEATURE_LEVEL_10_1:    return TEXT("10_1");
    case D3D_FEATURE_LEVEL_11_0:    return TEXT("11_0");
    case D3D_FEATURE_LEVEL_11_1:    return TEXT("11_1");
    }
    return TEXT("X_X");
}

static void LogDXGIAdapterDesc(const DXGI_ADAPTER_DESC& AdapterDesc)
{
    E_LOG(Log, TEXT("    Description : {}"), AdapterDesc.Description);
    E_LOG(Log, TEXT("    VendorId    : {:#04x}"), AdapterDesc.VendorId);
    E_LOG(Log, TEXT("    DeviceId    : {:#04x}"), AdapterDesc.DeviceId);
    E_LOG(Log, TEXT("    SubSysId    : {:#04x}"), AdapterDesc.SubSysId);
    E_LOG(Log, TEXT("    Revision    : {:#04x}"), AdapterDesc.Revision);
    E_LOG(Log, TEXT("    DedicatedVideoMemory : {} bytes"), AdapterDesc.DedicatedVideoMemory);
    E_LOG(Log, TEXT("    DedicatedSystemMemory : {} bytes"), AdapterDesc.DedicatedSystemMemory);
    E_LOG(Log, TEXT("    SharedSystemMemory : {} bytes"), AdapterDesc.SharedSystemMemory);
    E_LOG(Log, TEXT("    AdapterLuid : {} {}"), AdapterDesc.AdapterLuid.HighPart, AdapterDesc.AdapterLuid.LowPart);
}


void FD3D11DynamicRHIModule::StartupModule()
{
}

bool FD3D11DynamicRHIModule::IsSupported()
{
    // if not computed yet
    if (!ChosenAdapter.IsValid())
    {
        FindAdapter();
    }

    // The hardware must support at least 11_0.
    return ChosenAdapter.IsValid()
        && ChosenAdapter.MaxSupportedFeatureLevel >= D3D_FEATURE_LEVEL_11_0;
}

FDynamicRHI* FD3D11DynamicRHIModule::CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel)
{
    return nullptr;
}

void FD3D11DynamicRHIModule::FindAdapter()
{
    // 일단 하나를 선택하면 다시 할 필요가 없습니다.
    _ASSERT(!ChosenAdapter.IsValid());

    // DXGIFactory1을 생성하려고 시도합니다. Vista SP2 이상을 실행하지 않는 경우 실패합니다.
    TRefCountPtr<IDXGIFactory1> DXGIFactory1;
    SafeCreateDXGIFactory(DXGIFactory1.GetInitReference(), WITH_DEBUG);
    if (!DXGIFactory1)
    {
        return;
    }

    TRefCountPtr<IDXGIFactory6> DXGIFactory6;
    DXGIFactory1->QueryInterface(__uuidof(IDXGIFactory6), (void**)DXGIFactory6.GetInitReference());


    // 우리는 Perf 안쓸꺼
    bool bAllowPerfHUD = false;// true;
    //#if UE_BUILD_SHIPPING || UE_BUILD_TEST
    //    bAllowPerfHUD = false;
    //#endif

    TRefCountPtr<IDXGIAdapter> TempAdapter;
    D3D_FEATURE_LEVEL MinAllowedFeatureLevel = GetMinAllowedD3DFeatureLevel();
    D3D_FEATURE_LEVEL MaxAllowedFeatureLevel = GetMaxAllowedD3DFeatureLevel();

    E_LOG(Log, TEXT("D3D11 min allowed feature level: {}"), GetFeatureLevelString(MinAllowedFeatureLevel));
    E_LOG(Log, TEXT("D3D11 max allowed feature level: {}"), GetFeatureLevelString(MaxAllowedFeatureLevel));

    FD3D11Adapter PreferredAdapter;
    FD3D11Adapter BestMemoryAdapter;
    FD3D11Adapter FirstDiscreteAdapter;
    FD3D11Adapter FirstAdapter;
    SIZE_T BestDedicatedMemory = 0;

    E_LOG(Log, TEXT("D3D11 adapters:"));

    const EGpuVendorId PreferredVendor = RHIGetPreferredAdapterVendor();
    bool bAllowSoftwareFallback = false;// D3D11RHI_AllowSoftwareFallback();


    int GpuPreferenceInt = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE;
    // FParse::Value(FCommandLine::Get(), TEXT("-gpupreference="), GpuPreferenceInt);
    DXGI_GPU_PREFERENCE GpuPreference;
    switch (GpuPreferenceInt)
    {
    case 1: GpuPreference = DXGI_GPU_PREFERENCE_MINIMUM_POWER; break;
    case 2: GpuPreference = DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE; break;
    default: GpuPreference = DXGI_GPU_PREFERENCE_UNSPECIFIED; break;
    }

    auto LocalEnumAdapters = [&DXGIFactory6, &DXGIFactory1, GpuPreference](UINT AdapterIndex, IDXGIAdapter** Adapter) -> HRESULT
        {
            if (!DXGIFactory6 || GpuPreference == DXGI_GPU_PREFERENCE_UNSPECIFIED)
            {
                return DXGIFactory1->EnumAdapters(AdapterIndex, Adapter);
            }
            else
            {
                return DXGIFactory6->EnumAdapterByGpuPreference(AdapterIndex, GpuPreference, __uuidof(IDXGIAdapter), (void**)Adapter);
            }
        };

    // DXGIFactory의 어댑터를 열거합니다.
    for (uint32 AdapterIndex = 0; LocalEnumAdapters(AdapterIndex, TempAdapter.GetInitReference()) != DXGI_ERROR_NOT_FOUND; ++AdapterIndex)
    {
        // Check that if adapter supports D3D11.
        if (TempAdapter)
        {
            E_LOG(Log, TEXT("Testing D3D11 Adapter {}:"), AdapterIndex);
            DXGI_ADAPTER_DESC AdapterDesc;
            if (HRESULT DescResult = TempAdapter->GetDesc(&AdapterDesc); FAILED(DescResult))
            {
                E_LOG(Warning, TEXT("Failed to get description for adapter {}."), AdapterIndex);
            }
            else
            {
                LogDXGIAdapterDesc(AdapterDesc);
            }


            //D3D_FEATURE_LEVEL ActualFeatureLevel = (D3D_FEATURE_LEVEL)0;
            //if (SafeTestD3D11CreateDevice(TempAdapter.Get(), MinAllowedFeatureLevel, MaxAllowedFeatureLevel, &ActualFeatureLevel))
            //{
            //    // Log some information about the available D3D11 adapters.

            //    uint32 OutputCount = CountAdapterOutputs(TempAdapter);

            //    E_LOG(Info,
            //        TEXT("  {}. '{}' (Feature Level {})"),
            //        AdapterIndex,
            //        AdapterDesc.Description,
            //        GetFeatureLevelString(ActualFeatureLevel)
            //    );
            //    E_LOG(Info,
            //        TEXT("      {}/{}/{} MB DedicatedVideo/DedicatedSystem/SharedSystem, Outputs:{}, VendorId:{:#x}"),
            //        (uint32)(AdapterDesc.DedicatedVideoMemory / (1024 * 1024)),
            //        (uint32)(AdapterDesc.DedicatedSystemMemory / (1024 * 1024)),
            //        (uint32)(AdapterDesc.SharedSystemMemory / (1024 * 1024)),
            //        OutputCount,
            //        AdapterDesc.VendorId
            //    );

            //    const bool bIsWARP = RHIConvertToGpuVendorId(AdapterDesc.VendorId) == EGpuVendorId::Microsoft;

            //    // Simple heuristic but without profiling it's hard to do better
            //    bool bIsNonLocalMemoryPresent = false;
            //    ComPtr<IDXGIAdapter3> TempDxgiAdapter3;
            //    DXGI_QUERY_VIDEO_MEMORY_INFO NonLocalVideoMemoryInfo;
            //    if (SUCCEEDED(TempAdapter->QueryInterface(_uuidof(IDXGIAdapter3), (void**)TempDxgiAdapter3.ReleaseAndGetAddressOf())) &&
            //        TempDxgiAdapter3.Get() && SUCCEEDED(TempDxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &NonLocalVideoMemoryInfo)))
            //    {
            //        bIsNonLocalMemoryPresent = NonLocalVideoMemoryInfo.Budget != 0;
            //    }

            //    // TODO: Using GPUDetect for Intel GPUs to check for integrated vs discrete status, pending GPUDetect update

            //    const bool bIsIntegrated = !bIsNonLocalMemoryPresent;
            //    // PerfHUD is for performance profiling
            //    const bool bIsPerfHUD = !std::wcscmp(AdapterDesc.Description, TEXT("NVIDIA PerfHUD"));

            //    FD3D11Adapter CurrentAdapter(TempAdapter, ActualFeatureLevel, bIsWARP, bIsIntegrated);

            //    // Add special check to support HMDs, which do not have associated outputs.
            //    // To reject the software emulation, unless the cvar wants it.
            //    // https://msdn.microsoft.com/en-us/library/windows/desktop/bb205075(v=vs.85).aspx#WARP_new_for_Win8
            //    // Before we tested for no output devices but that failed where a laptop had a Intel (with output) and NVidia (with no output)
            //    const bool bSkipSoftwareAdapter = bIsWARP && !bAllowSoftwareFallback /*&& CVarExplicitAdapterValue < 0 && HmdGraphicsAdapterLuid == 0*/;

            //    // we don't allow the PerfHUD adapter
            //    const bool bSkipPerfHUDAdapter = bIsPerfHUD && !bAllowPerfHUD;

            //    // the HMD wants a specific adapter, not this one
            //    //const bool bSkipHmdGraphicsAdapter = HmdGraphicsAdapterLuid != 0 && FMemory::Memcmp(&HmdGraphicsAdapterLuid, &AdapterDesc.AdapterLuid, sizeof(LUID)) != 0;

            //    // the user wants a specific adapter, not this one
            //    //const bool bSkipExplicitAdapter = CVarExplicitAdapterValue >= 0 && AdapterIndex != CVarExplicitAdapterValue;

            //    const bool bSkipAdapter = bSkipSoftwareAdapter || bSkipPerfHUDAdapter /*|| bSkipHmdGraphicsAdapter || bSkipExplicitAdapter*/;

            //    if (!bSkipAdapter && CurrentAdapter.IsValid())
            //    {
            //        if (PreferredVendor != EGpuVendorId::Unknown && PreferredVendor == RHIConvertToGpuVendorId(AdapterDesc.VendorId) && !PreferredAdapter.IsValid())
            //        {
            //            PreferredAdapter = CurrentAdapter;
            //        }

            //        if (!bIsWARP && !CurrentAdapter.bIsIntegrated)
            //        {
            //            if (!FirstDiscreteAdapter.IsValid())
            //            {
            //                FirstDiscreteAdapter = CurrentAdapter;
            //            }

            //            if (AdapterDesc.DedicatedVideoMemory > BestDedicatedMemory)
            //            {
            //                BestMemoryAdapter = CurrentAdapter;
            //                BestDedicatedMemory = AdapterDesc.DedicatedVideoMemory;
            //                if (PreferredVendor != EGpuVendorId::Unknown && PreferredVendor == RHIConvertToGpuVendorId(AdapterDesc.VendorId))
            //                {
            //                    // Choose the best option of the preferred IHV devices
            //                    PreferredAdapter = BestMemoryAdapter;
            //                }
            //            }
            //        }

            //        if (!FirstAdapter.IsValid())
            //        {
            //            FirstAdapter = CurrentAdapter;
            //        }
            //    }
            //}
            //else
            //{
            //    E_LOG(Info, TEXT("  {}. Unknown, failed to create test device."), AdapterIndex);
            //}
        }
        else
        {
            E_LOG(Log, TEXT("  {}. Unknown, failed to create adapter."), AdapterIndex);
        }
    }

    //if (true /*bFavorDiscreteAdapter*/)
    //{
    //    if (PreferredAdapter.IsValid())
    //    {
    //        ChosenAdapter = PreferredAdapter;
    //    }
    //    else if (BestMemoryAdapter.IsValid())
    //    {
    //        ChosenAdapter = BestMemoryAdapter;
    //    }
    //    else if (FirstDiscreteAdapter.IsValid())
    //    {
    //        ChosenAdapter = FirstDiscreteAdapter;
    //    }
    //    else
    //    {
    //        ChosenAdapter = FirstAdapter;
    //    }
    //}
    //else
    //{
    //    ChosenAdapter = FirstAdapter;
    //}

    //if (ChosenAdapter.IsValid())
    //{
    //    E_LOG(Info, TEXT("Chosen D3D11 Adapter:"));
    //    LogDXGIAdapterDesc(ChosenAdapter.DXGIAdapterDesc);
    //}
    //else
    //{
    //    E_LOG(Error, TEXT("Failed to choose a D3D11 Adapter."));
    //}

    //GRHIAdapterName = ChosenAdapter.DXGIAdapterDesc.Description;
    //GRHIVendorId = ChosenAdapter.DXGIAdapterDesc.VendorId;
    //GRHIDeviceId = ChosenAdapter.DXGIAdapterDesc.DeviceId;
    //GRHIDeviceRevision = ChosenAdapter.DXGIAdapterDesc.Revision;
    //GRHIDeviceIsIntegrated = ChosenAdapter.bIsIntegrated;
}