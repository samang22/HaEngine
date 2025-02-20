#include "Materials/Material.h"

IMPLEMENT_SHADER_TYPE(FMaterialVS, FPaths::EngineConfigDir() + L"/VertexShader.hlsl", "VS", SF_Vertex)
IMPLEMENT_SHADER_TYPE(FMaterialPS, FPaths::EngineConfigDir() + L"/PixelShader.hlsl", "PS", SF_Pixel)

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