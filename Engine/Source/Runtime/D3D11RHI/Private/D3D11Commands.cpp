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
        _ASSERT(false);
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

void FD3D11DynamicRHI::RHISetStreamSource(uint32 StreamIndex, FRHIBuffer* VertexBufferRHI, uint32 Offset)
{
    FD3D11Buffer* VertexBuffer = ResourceCast(VertexBufferRHI);

    ID3D11Buffer* D3DBuffer = VertexBuffer ? VertexBuffer->Resource.GetReference() : nullptr;
    TrackResourceBoundAsVB(VertexBuffer, StreamIndex);
    StateCache.SetStreamSource(D3DBuffer, StreamIndex, Offset);
}


static D3D11_PRIMITIVE_TOPOLOGY GetD3D11PrimitiveType(EPrimitiveType PrimitiveType)
{
    switch (PrimitiveType)
    {
    case PT_TriangleList: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
    case PT_TriangleStrip: return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP;
    case PT_LineList: return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
    case PT_PointList: return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;

    default: E_LOG(Fatal, TEXT("Unknown primitive type: {}"), (int32)PrimitiveType);
    };

    return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
}

void FD3D11DynamicRHI::RHISetPrimitiveTopology(EPrimitiveType InPrimitiveType)
{
    PrimitiveType = InPrimitiveType;
    //StateCache.SetPrimitiveTopology(GetD3D11PrimitiveType(InPrimitiveType));
}


void FD3D11DynamicRHI::RHIDrawPrimitive(uint32 BaseVertexIndex, uint32 NumPrimitives, uint32 NumInstances)
{
    // TODO
    //CommitGraphicsResourceTables();
    //CommitNonComputeShaderConstants();

    uint32 VertexCount = GetVertexCountForPrimitiveCount(NumPrimitives, PrimitiveType);

    //GPUProfilingData.RegisterGPUWork(NumPrimitives * NumInstances, VertexCount * NumInstances);
    StateCache.SetPrimitiveTopology(GetD3D11PrimitiveType(PrimitiveType));
    if (NumInstances > 1)
    {
        Direct3DDeviceIMContext->DrawInstanced(VertexCount, NumInstances, BaseVertexIndex, 0);
    }
    else
    {
        Direct3DDeviceIMContext->Draw(VertexCount, BaseVertexIndex);
    }

    //EnableUAVOverlap();
}


// Rasterizer state.
void FD3D11DynamicRHI::RHISetRasterizerState(FRHIRasterizerState* NewStateRHI)
{
    FD3D11RasterizerState* NewState = ResourceCast(NewStateRHI);
    StateCache.SetRasterizerState(NewState->Resource);
}

void FD3D11DynamicRHI::ValidateExclusiveDepthStencilAccess(FExclusiveDepthStencil RequestedAccess) const
{
    const bool bSrcDepthWrite = RequestedAccess.IsDepthWrite();
    const bool bSrcStencilWrite = RequestedAccess.IsStencilWrite();

    if (bSrcDepthWrite || bSrcStencilWrite)
    {
        // 새로운 규칙: SetRenderTarget[s]()를 먼저 호출해야 합니다
        _ASSERT(CurrentDepthTexture);

        const bool bDstDepthWrite = CurrentDSVAccessType.IsDepthWrite();
        const bool bDstStencilWrite = CurrentDSVAccessType.IsStencilWrite();

        // 요청된 접근이 불가능합니다. SetRenderTarget의 EExclusiveDepthStencil을 수정하거나 다른 것을 요청하십시오.
        _ASSERT(
            !bSrcDepthWrite || bDstDepthWrite,
            TEXT("기대값: SrcDepthWrite := false 또는 DstDepthWrite := true. 실제값: SrcDepthWrite := %s 또는 DstDepthWrite := %s"),
            (bSrcDepthWrite) ? TEXT("true") : TEXT("false"),
            (bDstDepthWrite) ? TEXT("true") : TEXT("false")
        );

        _ASSERT(
            !bSrcStencilWrite || bDstStencilWrite,
            TEXT("기대값: SrcStencilWrite := false 또는 DstStencilWrite := true. 실제값: SrcStencilWrite := %s 또는 DstStencilWrite := %s"),
            (bSrcStencilWrite) ? TEXT("true") : TEXT("false"),
            (bDstStencilWrite) ? TEXT("true") : TEXT("false")
        );

    }
}

void FD3D11DynamicRHI::RHISetDepthStencilState(FRHIDepthStencilState* NewStateRHI, uint32 StencilRef)
{
    FD3D11DepthStencilState* NewState = ResourceCast(NewStateRHI);

    ValidateExclusiveDepthStencilAccess(NewState->AccessType);

    StateCache.SetDepthStencilState(NewState->Resource, StencilRef);
}


void FD3D11DynamicRHI::RHIDrawIndexedPrimitive(FRHIBuffer* IndexBufferRHI, int32 BaseVertexIndex, uint32 FirstInstance, uint32 NumVertices, uint32 StartIndex, uint32 NumPrimitives, uint32 NumInstances)
{
    // [깊이 반전(근평면 1.f)] 임시로 여기서 일괄 처리
    //{
    //    // 반전된 깊이 스텐실 상태 설정
    //    D3D11_DEPTH_STENCIL_DESC invertedDepthStencilDesc;
    //    ZeroMemory(&invertedDepthStencilDesc, sizeof(invertedDepthStencilDesc));
    //    invertedDepthStencilDesc.DepthEnable = TRUE;
    //    invertedDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    //    invertedDepthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER; // 반전된 깊이 비교 함수

    //    TRefCountPtr<ID3D11DepthStencilState> invertedDepthStencilState;
    //    Direct3DDevice->CreateDepthStencilState(&invertedDepthStencilDesc, invertedDepthStencilState.GetInitReference());
    //    Direct3DDeviceIMContext->OMSetDepthStencilState(invertedDepthStencilState, 1);
    //}

    FD3D11Buffer* IndexBuffer = ResourceCast(IndexBufferRHI);

    // 호출된 함수는 입력이 유효한지 확인해야 합니다. 이는 숨겨진 버그를 방지합니다.
    _ASSERT(NumPrimitives > 0);

    //CommitGraphicsResourceTables();
    //CommitNonComputeShaderConstants();

    // determine 16bit vs 32bit indices
    const DXGI_FORMAT Format = (IndexBuffer->GetStride() == sizeof(uint16) ? DXGI_FORMAT_R16_UINT : DXGI_FORMAT_R32_UINT);

    uint32 IndexCount = GetVertexCountForPrimitiveCount(NumPrimitives, PrimitiveType);

    // 인덱스 버퍼 범위 밖을 읽으려고 하지 않는지 확인합니다.
    // 테스트는 다음의 최적화된 버전입니다: StartIndex + IndexCount <= IndexBuffer->GetSize() / IndexBuffer->GetStride()
    _ASSERT((StartIndex + IndexCount) * IndexBuffer->GetStride() <= IndexBuffer->GetSize(),
        TEXT("Start %u, Count %u, Type %u, Buffer Size %u, Buffer stride %u"), StartIndex, IndexCount, PrimitiveType, IndexBuffer->GetSize(), IndexBuffer->GetStride());

    TrackResourceBoundAsIB(IndexBuffer);
    StateCache.SetIndexBuffer(IndexBuffer->Resource, Format, 0);
    StateCache.SetPrimitiveTopology(GetD3D11PrimitiveType(PrimitiveType));

    if (NumInstances > 1 || FirstInstance != 0)
    {
        const uint64 TotalIndexCount = (uint64)NumInstances * (uint64)IndexCount + (uint64)StartIndex;
        _ASSERT(TotalIndexCount <= (uint64)0xFFFFFFFF, TEXT("Instanced Index Draw exceeds maximum d3d11 limit: Total: %llu, NumInstances: %llu, IndexCount: %llu, StartIndex: %llu, FirstInstance: %llu"), TotalIndexCount, NumInstances, IndexCount, StartIndex, FirstInstance);
        Direct3DDeviceIMContext->DrawIndexedInstanced(IndexCount, NumInstances, StartIndex, BaseVertexIndex, FirstInstance);
    }
    else
    {
        Direct3DDeviceIMContext->DrawIndexed(IndexCount, StartIndex, BaseVertexIndex);
    }

    //EnableUAVOverlap();
}

static void ValidateScissorRect(const D3D11_VIEWPORT& Viewport, const D3D11_RECT& ScissorRect)
{
    _ASSERT(ScissorRect.left >= (LONG)Viewport.TopLeftX);
    _ASSERT(ScissorRect.top >= (LONG)Viewport.TopLeftY);
    _ASSERT(ScissorRect.right <= (LONG)Viewport.TopLeftX + (LONG)Viewport.Width);
    _ASSERT(ScissorRect.bottom <= (LONG)Viewport.TopLeftY + (LONG)Viewport.Height);
    _ASSERT(ScissorRect.left <= ScissorRect.right && ScissorRect.top <= ScissorRect.bottom);
}

void FD3D11DynamicRHI::RHISetViewport(float MinX, float MinY, float MinZ, float MaxX, float MaxY, float MaxZ)
{
    // These are the maximum viewport extents for D3D11. Exceeding them leads to badness.
    _ASSERT(MinX <= (float)D3D11_VIEWPORT_BOUNDS_MAX);
    _ASSERT(MinY <= (float)D3D11_VIEWPORT_BOUNDS_MAX);
    _ASSERT(MaxX <= (float)D3D11_VIEWPORT_BOUNDS_MAX);
    _ASSERT(MaxY <= (float)D3D11_VIEWPORT_BOUNDS_MAX);

    D3D11_VIEWPORT Viewport = { MinX, MinY, MaxX - MinX, MaxY - MinY, MinZ, MaxZ };
    //avoid setting a 0 extent viewport, which the debug runtime doesn't like
    if (Viewport.Width > 0 && Viewport.Height > 0)
    {
        StateCache.SetViewport(Viewport);
        RHISetScissorRect(true, MinX, MinY, MaxX, MaxY);
    }
}

void FD3D11DynamicRHI::RHISetScissorRect(bool bEnable, uint32 MinX, uint32 MinY, uint32 MaxX, uint32 MaxY)
{
    D3D11_VIEWPORT Viewport;
    StateCache.GetViewport(&Viewport);

    D3D11_RECT ScissorRect;
    if (bEnable)
    {
        ScissorRect.left = MinX;
        ScissorRect.top = MinY;
        ScissorRect.right = MaxX;
        ScissorRect.bottom = MaxY;
    }
    else
    {
        ScissorRect.left = (LONG)Viewport.TopLeftX;
        ScissorRect.top = (LONG)Viewport.TopLeftY;
        ScissorRect.right = (LONG)Viewport.TopLeftX + (LONG)Viewport.Width;
        ScissorRect.bottom = (LONG)Viewport.TopLeftY + (LONG)Viewport.Height;
    }

    ValidateScissorRect(Viewport, ScissorRect);
    Direct3DDeviceIMContext->RSSetScissorRects(1, &ScissorRect);
}

unordered_map<FString, FTextureRHIRef> Textures;

FTextureRHIRef FD3D11DynamicRHI::RHICreateTexture(FRHICommandList& RHICmdList, const FRHITextureCreateDesc& CreateDesc)
{
    const FString TextureName = CreateDesc.DebugName;
    if (!TextureName.empty())
    {
        if (Textures.contains(TextureName))
        {
            FTextureRHIRef Texture = Textures[TextureName];

            FD3D11Texture* D3D11Texture = ResourceCast(Texture);
            const FVector3D TextureSize = D3D11Texture->GetSizeXYZ();
            const FVector3D NewTextureSize = CreateDesc.GetSize();
            if (TextureSize != CreateDesc.GetSize())
            {
                E_LOG(Warning, TEXT("{} size changed: {} {} {} to {} {} {}"),
                    TextureName,
                    TextureSize.x, TextureSize.y, TextureSize.z,
                    NewTextureSize.x, NewTextureSize.y, NewTextureSize.z);
                Textures.erase(TextureName);
            }
            else
            {
                return Texture;
            }
        }
    }

    FTextureRHIRef NewTexture = CreateDesc.IsTexture3D()
        ? nullptr // CreateD3D11Texture3D(CreateDesc)
        : CreateD3D11Texture2D(CreateDesc);

    if (!TextureName.empty())
    {
        Textures.emplace(TextureName, NewTexture);
    }

    return NewTexture;
}