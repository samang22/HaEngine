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
protected:
	ID3D11DeviceContext* Direct3DDeviceIMContext;

    // Input Layout State
    ID3D11InputLayout* CurrentInputLayout;

    // Shader Cache
    ID3D11VertexShader* CurrentVertexShader;
    //ID3D11GeometryShader* CurrentGeometryShader;
    ID3D11PixelShader* CurrentPixelShader;
    //ID3D11ComputeShader* CurrentComputeShader;

    uint16 StreamStrides[MaxVertexElementCount];
};