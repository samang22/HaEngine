#include "Materials/Material.h"

IMPLEMENT_SHADER_TYPE(FMaterialVS, FPaths::ShaderDir() + L"/MaterialShader.hlsl", "VS", SF_Vertex)
IMPLEMENT_SHADER_TYPE(FMaterialPS, FPaths::ShaderDir() + L"/MaterialShader.hlsl", "PS", SF_Pixel)

void UMaterial::SetVertexShader(TShaderMapRef<FMaterialVS> InShader)
{
    VertexShader = InShader;
}

void UMaterial::SetPixelShader(TShaderMapRef<FMaterialPS> InShader)
{
    PixelShader = InShader;
}

FRHIVertexShader* UMaterial::GetVertexShaderRHI() const
{
    return VertexShader.GetVertexShader();
}

FRHIPixelShader* UMaterial::GetPixelShaderRHI() const
{
    return PixelShader.GetPixelShader();
}