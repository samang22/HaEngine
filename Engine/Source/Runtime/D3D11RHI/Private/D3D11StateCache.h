#pragma once

class ID3D11DeviceContext;

//-----------------------------------------------------------------------------
//	FD3D11StateCache Class Definition
//-----------------------------------------------------------------------------
class FD3D11StateCache
{
public:
	void Init(ID3D11DeviceContext* InDeviceContext, bool bInAlwaysSetIndexBuffers = false)
	{
		SetContext(InDeviceContext);
	}

	virtual void SetContext(ID3D11DeviceContext* InDeviceContext)
	{
		Direct3DDeviceIMContext = InDeviceContext;
		ClearState();
	}

	/**
	 * 모든 D3D11 상태를 초기화하여 모든 입력/출력 리소스 슬롯, 셰이더, 입력 레이아웃,
	 * 예상 값, 가위 사각형, 깊이-스텐실 상태, 래스터라이저 상태, 블렌드 상태,
	 * 샘플러 상태 및 뷰포트를 NULL로 설정합니다.
	 */
	virtual void ClearState();

    inline void SetStreamStrides(const uint16* InStreamStrides)
    {
        memcpy(StreamStrides, InStreamStrides, sizeof(StreamStrides));
    }

    inline void SetInputLayout(ID3D11InputLayout* InputLayout)
    {
        if (CurrentInputLayout != InputLayout)
        {
            CurrentInputLayout = InputLayout;
            Direct3DDeviceIMContext->IASetInputLayout(InputLayout);
        }
    }

    inline void SetVertexShader(ID3D11VertexShader* Shader)
    {
        if (CurrentVertexShader != Shader)
        {
            CurrentVertexShader = Shader;
            Direct3DDeviceIMContext->VSSetShader(Shader, nullptr, 0);
        }
    }


    inline void SetPixelShader(ID3D11PixelShader* Shader)
    {
        if (CurrentPixelShader != Shader)
        {
            CurrentPixelShader = Shader;
            Direct3DDeviceIMContext->PSSetShader(Shader, nullptr, 0);
        }
    }

    inline void InternalSetStreamSource(ID3D11Buffer* VertexBuffer, uint32 StreamIndex, uint32 Stride, uint32 Offset)
    {
        _ASSERT(StreamIndex < ARRAYSIZE(CurrentVertexBuffers));
        FD3D11VertexBufferState& Slot = CurrentVertexBuffers[StreamIndex];
        if ((Slot.VertexBuffer != VertexBuffer || Slot.Offset != Offset || Slot.Stride != Stride) /*|| GD3D11SkipStateCaching*/)
        {
            Slot.VertexBuffer = VertexBuffer;
            Slot.Offset = Offset;
            Slot.Stride = Stride;
            Direct3DDeviceIMContext->IASetVertexBuffers(StreamIndex, 1, &VertexBuffer, &Stride, &Offset);
        }
    }


    inline void SetStreamSource(ID3D11Buffer* VertexBuffer, uint32 StreamIndex, uint32 Stride, uint32 Offset)
    {
        _ASSERT(Stride == StreamStrides[StreamIndex]);
        InternalSetStreamSource(VertexBuffer, StreamIndex, Stride, Offset);
    }

    inline void SetStreamSource(ID3D11Buffer* VertexBuffer, uint32 StreamIndex, uint32 Offset)
    {
        InternalSetStreamSource(VertexBuffer, StreamIndex, StreamStrides[StreamIndex], Offset);
    }

    inline void SetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY PrimitiveTopology)
    {
        if (CurrentPrimitiveTopology != PrimitiveTopology)
        {
            CurrentPrimitiveTopology = PrimitiveTopology;
            Direct3DDeviceIMContext->IASetPrimitiveTopology(PrimitiveTopology);
        }
    }


protected:
	ID3D11DeviceContext* Direct3DDeviceIMContext;

    // Input Layout State
    ID3D11InputLayout* CurrentInputLayout;

    // Shader Cache
    ID3D11VertexShader* CurrentVertexShader;
    //ID3D11GeometryShader* CurrentGeometryShader;
    ID3D11PixelShader* CurrentPixelShader;
    //ID3D11ComputeShader* CurrentComputeShader;

    // Vertex Buffer State
    struct FD3D11VertexBufferState
    {
        ID3D11Buffer* VertexBuffer;
        uint32 Stride;
        uint32 Offset;
    } CurrentVertexBuffers[D3D11_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT];

    // Primitive Topology State
    D3D11_PRIMITIVE_TOPOLOGY CurrentPrimitiveTopology;

    uint16 StreamStrides[MaxVertexElementCount];
};