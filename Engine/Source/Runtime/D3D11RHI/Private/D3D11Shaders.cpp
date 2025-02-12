#include "D3D11RHIPrivate.h"

map<type_index, TRefCountPtr<FRHIShader>> RHIShaders;

FVertexShaderRHIRef FD3D11DynamicRHI::RHICreateVertexShader(const TArray<uint8> Code, const type_index& Key)
{
    if (RHIShaders.contains(Key))
    {
        return (FVertexShaderRHIRef)RHIShaders[Key];
    }

    FD3D11VertexShader* Shader = new FD3D11VertexShader;
    VERIFYD3D11SHADERRESULT(Direct3DDevice->CreateVertexShader(Code.data(), Code.size(), nullptr, Shader->Resource.GetInitReference())
        , Shader, Direct3DDevice);
    Shader->Code = Code;

    FVertexShaderRHIRef ShaderRef = Shader;
    RHIShaders.emplace(Key, ShaderRef);

    return ShaderRef;
}

FPixelShaderRHIRef FD3D11DynamicRHI::RHICreatePixelShader(const TArray<uint8> Code, const type_index& Key)
{
    if (RHIShaders.contains(Key))
    {
        return (FPixelShaderRHIRef)RHIShaders[Key];
    }

    FD3D11PixelShader* Shader = new FD3D11PixelShader;
    VERIFYD3D11SHADERRESULT(Direct3DDevice->CreatePixelShader(Code.data(), Code.size(), nullptr, Shader->Resource.GetInitReference())
        , Shader, Direct3DDevice);

    FPixelShaderRHIRef ShaderRef = Shader;
    RHIShaders.emplace(Key, ShaderRef);

    return ShaderRef;
}