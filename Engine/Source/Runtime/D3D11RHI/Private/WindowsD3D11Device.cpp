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

/**
 * 최대 MaxFeatureLevel을 사용하여 어댑터에 대한 D3D11 장치를 생성하려고 시도합니다.
 * 생성이 성공하면 true를 반환하고 지원되는 기능 수준이 OutFeatureLevel에 설정됩니다.
 */
static bool SafeTestD3D11CreateDevice(IDXGIAdapter* Adapter, D3D_FEATURE_LEVEL MinFeatureLevel, D3D_FEATURE_LEVEL MaxFeatureLevel, D3D_FEATURE_LEVEL* OutFeatureLevel)
{
    ID3D11Device* D3DDevice = nullptr;
    ID3D11DeviceContext* D3DDeviceContext = nullptr;
    uint32 DeviceFlags = D3D11_CREATE_DEVICE_SINGLETHREADED;
    // Use a debug device if specified on the command line.
    if (WITH_DEBUG)//(GRHIGlobals.IsDebugLayerEnabled)
    {
        DeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
    }

    //// @MIXEDREALITY_CHANGE : BEGIN - Add BGRA flag for Windows Mixed Reality HMD's
    //DeviceFlags |= D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    //// @MIXEDREALITY_CHANGE : END

    D3D_FEATURE_LEVEL RequestedFeatureLevels[] =
    {
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0,
    };

    // Trim to allowed feature levels
    int32 FirstAllowedFeatureLevel = 0;
    int32 NumAllowedFeatureLevels = ARRAYSIZE(RequestedFeatureLevels);
    int32 LastAllowedFeatureLevel = NumAllowedFeatureLevels - 1;

    while (FirstAllowedFeatureLevel < NumAllowedFeatureLevels)
    {
        if (RequestedFeatureLevels[FirstAllowedFeatureLevel] == MaxFeatureLevel)
        {
            break;
        }
        FirstAllowedFeatureLevel++;
    }

    while (LastAllowedFeatureLevel > 0)
    {
        if (RequestedFeatureLevels[LastAllowedFeatureLevel] >= MinFeatureLevel)
        {
            break;
        }
        LastAllowedFeatureLevel--;
    }

    NumAllowedFeatureLevels = LastAllowedFeatureLevel - FirstAllowedFeatureLevel + 1;
    if (MaxFeatureLevel < MinFeatureLevel || NumAllowedFeatureLevels <= 0)
    {
        return false;
    }

    try
    {
        // 소프트웨어 렌더러를 원하지 않습니다. 이상적으로는 생성 시 D3D_DRIVER_TYPE_HARDWARE를 지정하지만,
        // 어댑터를 지정할 때는 D3D_DRIVER_TYPE_UNKNOWN을 지정해야 합니다 (그렇지 않으면 호출이 실패합니다).
        // 나중에 장치 유형을 확인할 수 없습니다 (D3D에서 이 기능이 누락된 것 같습니다).
        HRESULT Result = D3D11CreateDevice(
            Adapter,
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr,
            DeviceFlags,
            &RequestedFeatureLevels[FirstAllowedFeatureLevel],
            NumAllowedFeatureLevels,
            D3D11_SDK_VERSION,
            &D3DDevice,
            OutFeatureLevel,
            &D3DDeviceContext);

        if (SUCCEEDED(Result))
        {
            D3DDevice->Release();
            D3DDeviceContext->Release();
            return true;
        }

        // 테스트 장치 생성 실패의 이유를 로그로 기록합니다. 추가 디버그 도움이 됩니다.        VERIFYD3D11RESULT_NOEXIT(Result);
        //bool bIsWin10 = FPlatformMisc::VerifyWindowsVersion(10, 0);

        // Fatal error on 0x887A002D
        //if (DXGI_ERROR_SDK_COMPONENT_MISSING == Result && bIsWin10)
        //{
        //    UE_LOG(LogD3D11RHI, Fatal, TEXT("-d3ddebug was used but optional Graphics Tools were not found. Install them through the Manage Optional Features in windows. See: https://docs.microsoft.com/en-us/windows/uwp/gaming/use-the-directx-runtime-and-visual-studio-graphics-diagnostic-features"));
        //}
    }
    catch (...)
    {
        E_LOG(Error, TEXT("check"));
        // We suppress warning C6322: Empty _except block. Appropriate checks are made upon returning. 
        //CA_SUPPRESS(6322);
    }

    return false;
}

static uint32 CountAdapterOutputs(TRefCountPtr<IDXGIAdapter>& Adapter)
{
    uint32 OutputCount = 0;
    for (;;)
    {
        TRefCountPtr<IDXGIOutput> Output;
        HRESULT hr = Adapter->EnumOutputs(OutputCount, Output.GetInitReference());
        if (FAILED(hr))
        {
            break;
        }
        ++OutputCount;
    }

    return OutputCount;
}

FD3D11DynamicRHI* GD3D11RHI = nullptr;

void FD3D11DynamicRHIModule::StartupModule()
{
    // TODO: GD3D11RHI 소멸 시키기
    _ASSERT(false);
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
    IDXGIFactory1* DXGIFactory1;
    VERIFYD3D11RESULT(ChosenAdapter.DXGIAdapter->GetParent(__uuidof(DXGIFactory1), reinterpret_cast<void**>(&DXGIFactory1)));

    GD3D11RHI = new FD3D11DynamicRHI();
    return GD3D11RHI;
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

            D3D_FEATURE_LEVEL ActualFeatureLevel = (D3D_FEATURE_LEVEL)0;
            if (SafeTestD3D11CreateDevice(TempAdapter, MinAllowedFeatureLevel, MaxAllowedFeatureLevel, &ActualFeatureLevel))
            {
                // 사용 가능한 D3D11 어댑터에 대한 정보를 로그로 기록합니다.

                uint32 OutputCount = CountAdapterOutputs(TempAdapter);

                E_LOG(Log,
                    TEXT("  {}. '{}' (Feature Level {})"),
                    AdapterIndex,
                    AdapterDesc.Description,
                    GetFeatureLevelString(ActualFeatureLevel)
                );
                E_LOG(Log,
                    TEXT("      {}/{}/{} MB DedicatedVideo/DedicatedSystem/SharedSystem, Outputs:{}, VendorId:{:#x}"),
                    (uint32)(AdapterDesc.DedicatedVideoMemory / (1024 * 1024)),
                    (uint32)(AdapterDesc.DedicatedSystemMemory / (1024 * 1024)),
                    (uint32)(AdapterDesc.SharedSystemMemory / (1024 * 1024)),
                    OutputCount,
                    AdapterDesc.VendorId
                );

                // Windows Advanced Rasterization Platform(WARP)과 관련된 변수를 나타냅니다. 
                // WARP는 하드웨어 가속이 없는 시스템에서도 DirectX 기능을 소프트웨어 렌더링을 통해 지원하는 기술입니다.
                const bool bIsWARP = RHIConvertToGpuVendorId(AdapterDesc.VendorId) == EGpuVendorId::Microsoft;

                // 간단한 휴리스틱이지만 프로파일링 없이 더 나은 결과를 얻기 어렵습니다.
                bool bIsNonLocalMemoryPresent = false;
                TRefCountPtr<IDXGIAdapter3> TempDxgiAdapter3;
                DXGI_QUERY_VIDEO_MEMORY_INFO NonLocalVideoMemoryInfo;
                if (SUCCEEDED(TempAdapter->QueryInterface(_uuidof(IDXGIAdapter3), (void**)TempDxgiAdapter3.GetInitReference())) &&
                    TempDxgiAdapter3.IsValid() && SUCCEEDED(TempDxgiAdapter3->QueryVideoMemoryInfo(0, DXGI_MEMORY_SEGMENT_GROUP_NON_LOCAL, &NonLocalVideoMemoryInfo)))
                {
                    bIsNonLocalMemoryPresent = NonLocalVideoMemoryInfo.Budget != 0;
                }

                // TODO: GPUDetect를 사용하여 Intel GPU의 통합 여부와 개별 여부를 확인하는 작업을 GPUDetect 업데이트 대기 중에 수행합니다.

                const bool bIsIntegrated = !bIsNonLocalMemoryPresent;
                // PerfHUD는 성능 프로파일링을 위한 것입니다.
                const bool bIsPerfHUD = !std::wcscmp(AdapterDesc.Description, TEXT("NVIDIA PerfHUD"));

                FD3D11Adapter CurrentAdapter(TempAdapter, ActualFeatureLevel, bIsWARP, bIsIntegrated);

                // HMD(헤드 마운트 디스플레이)를 지원하기 위해 특별한 검사를 추가합니다. HMD는 관련 출력 장치가 없습니다.
                // 소프트웨어 에뮬레이션을 거부합니다, 단 cvar가 이를 원하지 않는 경우를 제외합니다.
                // https://msdn.microsoft.com/en-us/library/windows/desktop/bb205075(v=vs.85).aspx#WARP_new_for_Win8
                // 이전에는 출력 장치가 없는 경우를 테스트했으나, Intel(출력 있음)과 NVidia(출력 없음) GPU를 가진 노트북에서는 실패했습니다.
                const bool bSkipSoftwareAdapter = bIsWARP && !bAllowSoftwareFallback /*&& CVarExplicitAdapterValue < 0 && HmdGraphicsAdapterLuid == 0*/;

                // PerfHUD 어댑터를 허용하지 않습니다.
                const bool bSkipPerfHUDAdapter = bIsPerfHUD && !bAllowPerfHUD;

                // HMD는 이 어댑터가 아닌 특정 어댑터를 원합니다.
                //const bool bSkipHmdGraphicsAdapter = HmdGraphicsAdapterLuid != 0 && FMemory::Memcmp(&HmdGraphicsAdapterLuid, &AdapterDesc.AdapterLuid, sizeof(LUID)) != 0;

                // 사용자는 이 어댑터가 아닌 특정 어댑터를 원합니다.
                //const bool bSkipExplicitAdapter = CVarExplicitAdapterValue >= 0 && AdapterIndex != CVarExplicitAdapterValue;

                const bool bSkipAdapter = bSkipSoftwareAdapter || bSkipPerfHUDAdapter /*|| bSkipHmdGraphicsAdapter || bSkipExplicitAdapter*/;

                if (!bSkipAdapter && CurrentAdapter.IsValid())
                {
                    if (PreferredVendor != EGpuVendorId::Unknown && PreferredVendor == RHIConvertToGpuVendorId(AdapterDesc.VendorId) && !PreferredAdapter.IsValid())
                    {
                        PreferredAdapter = CurrentAdapter;
                    }

                    if (!bIsWARP && !CurrentAdapter.bIsIntegrated)
                    {
                        if (!FirstDiscreteAdapter.IsValid())
                        {
                            FirstDiscreteAdapter = CurrentAdapter;
                        }

                        if (AdapterDesc.DedicatedVideoMemory > BestDedicatedMemory)
                        {
                            BestMemoryAdapter = CurrentAdapter;
                            BestDedicatedMemory = AdapterDesc.DedicatedVideoMemory;
                            if (PreferredVendor != EGpuVendorId::Unknown && PreferredVendor == RHIConvertToGpuVendorId(AdapterDesc.VendorId))
                            {
                                // 선호하는 IHV 장치 중에서 최적의 옵션을 선택하세요.
                                PreferredAdapter = BestMemoryAdapter;
                            }
                        }
                    }

                    if (!FirstAdapter.IsValid())
                    {
                        FirstAdapter = CurrentAdapter;
                    }
                }
            }
            else
            { 
                E_LOG(Error, TEXT("  {}. Unknown, failed to create test device."), AdapterIndex);
            }
        
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