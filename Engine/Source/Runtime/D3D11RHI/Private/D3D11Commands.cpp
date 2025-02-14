#include "D3D11RHIPrivate.h"

IRHICommandContext* FD3D11DynamicRHI::RHIGetDefaultContext()
{
	return this;
}

/**
 * 바운드 셰이더 상태를 설정합니다. 이 작업은 버텍스 선언/셰이더 및 픽셀 셰이더를 설정합니다.
 * @param BoundShaderState - 상태 리소스
 */
void FD3D11DynamicRHI::RHISetBoundShaderState(FRHIBoundShaderState* BoundShaderStateRHI)
{
    FD3D11BoundShaderState* BoundShaderState = ResourceCast(BoundShaderStateRHI);

    StateCache.SetStreamStrides(BoundShaderState->StreamStrides);
    StateCache.SetInputLayout(BoundShaderState->InputLayout);
    StateCache.SetVertexShader(BoundShaderState->VertexShader);
    StateCache.SetPixelShader(BoundShaderState->PixelShader);

    //StateCache.SetGeometryShader(BoundShaderState->GeometryShader);

    //// @TODO : really should only discard the constants if the shader state has actually changed.
    //bDiscardSharedConstants = true;

    //// Prevent transient bound shader states from being recreated for each use by keeping a history of the most recently used bound shader states.
    //// The history keeps them alive, and the bound shader state cache allows them to am be reused if needed.
    //BoundShaderStateHistory.Add(BoundShaderState);

    //// Shader changed so all resource tables are dirty
    //DirtyUniformBuffers[SF_Vertex] = 0xffff;
    //DirtyUniformBuffers[SF_Pixel] = 0xffff;
    //DirtyUniformBuffers[SF_Geometry] = 0xffff;

    //// Shader changed.  All UB's must be reset by high level code to match other platforms anway.
    //// Clear to catch those bugs, and bugs with stale UB's causing layout mismatches.
    //// Release references to bound uniform buffers.
    //for (int32 Frequency = 0; Frequency < SF_NumStandardFrequencies; ++Frequency)
    //{
    //    for (int32 BindIndex = 0; BindIndex < MAX_UNIFORM_BUFFERS_PER_SHADER_STAGE; ++BindIndex)
    //    {
    //        BoundUniformBuffers[Frequency][BindIndex] = nullptr;
    //    }
    //}

    //if (GUnbindResourcesBetweenDrawsInDX11 || GRHIGlobals.IsDebugLayerEnabled)
    //{
    //    ClearAllShaderResources();
    //}
}

FBufferRHIRef FD3D11DynamicRHI::RHICreateBuffer(FRHICommandList& RHICmdList, FRHIBufferDesc const& BufferDesc, ERHIAccess ResourceState, FRHIResourceCreateInfo& CreateInfo)
{
    if (BufferDesc.IsNull())
    {
        return new FD3D11Buffer(nullptr, BufferDesc);
    }

    // CreateBuffer가 명확하지 않게 실패하기 전에 크기가 0이 아닌지 명시적으로 확인합니다.
    _ASSERT(BufferDesc.Size > 0, TEXT("크기 0으로 버퍼 '%s'를 생성하려고 시도했습니다."), CreateInfo.DebugName ? CreateInfo.DebugName : TEXT("(null)"));

    // 버퍼를 설명합니다.
    D3D11_BUFFER_DESC Desc{};
    Desc.ByteWidth = BufferDesc.Size;

    if (EnumHasAnyFlags(BufferDesc.Usage, BUF_AnyDynamic))
    {
        Desc.Usage = D3D11_USAGE_DYNAMIC;
        Desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    }

    if (EnumHasAnyFlags(BufferDesc.Usage, BUF_VertexBuffer))
    {
        Desc.BindFlags |= D3D11_BIND_VERTEX_BUFFER;
    }

    if (EnumHasAnyFlags(BufferDesc.Usage, BUF_IndexBuffer))
    {
        Desc.BindFlags |= D3D11_BIND_INDEX_BUFFER;
    }

    if (EnumHasAnyFlags(BufferDesc.Usage, BUF_ByteAddressBuffer))
    {
        Desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_ALLOW_RAW_VIEWS;
    }
    else if (EnumHasAnyFlags(BufferDesc.Usage, BUF_StructuredBuffer))
    {
        Desc.StructureByteStride = BufferDesc.Stride;
        Desc.MiscFlags |= D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
    }

    if (EnumHasAnyFlags(BufferDesc.Usage, BUF_ShaderResource))
    {
        Desc.BindFlags |= D3D11_BIND_SHADER_RESOURCE;
    }

    if (EnumHasAnyFlags(BufferDesc.Usage, BUF_UnorderedAccess))
    {
        Desc.BindFlags |= D3D11_BIND_UNORDERED_ACCESS;
    }

    if (EnumHasAnyFlags(BufferDesc.Usage, BUF_DrawIndirect))
    {
        Desc.MiscFlags |= D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;
    }

    if (EnumHasAnyFlags(BufferDesc.Usage, BUF_Shared))
    {
        /*if (GCVarUseSharedKeyedMutex->GetInt() != 0)
        {
            Desc.MiscFlags |= D3D11_RESOURCE_MISC_SHARED_KEYEDMUTEX;
        }
        else*/
        {
            Desc.MiscFlags |= D3D11_RESOURCE_MISC_SHARED;
        }
    }

    // If a resource array was provided for the resource, create the resource pre-populated
    D3D11_SUBRESOURCE_DATA InitData;
    D3D11_SUBRESOURCE_DATA* pInitData = NULL;
    if (CreateInfo.ResourceArray)
    {
        _ASSERT(BufferDesc.Size == CreateInfo.ResourceArray->GetResourceDataSize());
        InitData.pSysMem = CreateInfo.ResourceArray->GetResourceData();
        InitData.SysMemPitch = BufferDesc.Size;
        InitData.SysMemSlicePitch = 0;
        pInitData = &InitData;
    }

    TRefCountPtr<ID3D11Buffer> BufferResource;
    {
        HRESULT hr = Direct3DDevice->CreateBuffer(&Desc, pInitData, BufferResource.GetInitReference());
        if (FAILED(hr))
        {
            /*E_LOG(Error, TEXT("Failed to create buffer '{}' with ByteWidth={}, Usage={}, BindFlags=0x{:x}, CPUAccessFlags=0x{:x}, MiscFlags=0x{:x}, StructureByteStride={}, InitData=0x{:x}"),
                CreateInfo.DebugName ? CreateInfo.DebugName : TEXT(""), Desc.ByteWidth, Desc.Usage, Desc.BindFlags, Desc.CPUAccessFlags, Desc.MiscFlags, Desc.StructureByteStride, pInitData);*/
            VerifyD3D11Result(hr, "CreateBuffer", __FILE__, __LINE__, Direct3DDevice);
            _ASSERT(false);
        }
    }

    FString DebugName = CreateInfo.DebugName;
    if (CreateInfo.DebugName)
    {
        BufferResource->SetPrivateData(WKPDID_D3DDebugObjectName, wcslen(CreateInfo.DebugName) + 1, TCHAR_TO_ANSI(DebugName).data());
    }

    if (CreateInfo.DebugName)
    {
        BufferResource->SetPrivateData(WKPDID_D3DDebugObjectName, wcslen(CreateInfo.DebugName) + 1, TCHAR_TO_ANSI(DebugName).data());
    }

    if (CreateInfo.ResourceArray)
    {
        // 리소스 배열의 내용을 폐기합니다.
        CreateInfo.ResourceArray->Discard();
    }

    FD3D11Buffer* NewBuffer = new FD3D11Buffer(BufferResource, BufferDesc);
    if (CreateInfo.DebugName)
    {
        NewBuffer->SetName(CreateInfo.DebugName);
    }

    //D3D11BufferStats::UpdateBufferStats(*NewBuffer, true);

    return NewBuffer;
}