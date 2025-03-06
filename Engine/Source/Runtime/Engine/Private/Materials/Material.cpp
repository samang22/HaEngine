#include "Materials/Material.h"
#include "RHIStaticStates.h"

IMPLEMENT_SHADER_TYPE(FMaterialVS, FPaths::ShaderDir() + L"/MaterialShader.hlsl", "VS", SF_Vertex)
IMPLEMENT_SHADER_TYPE(FMaterialPS, FPaths::ShaderDir() + L"/MaterialShader.hlsl", "PS", SF_Pixel)

UMaterial::UMaterial()
{
    SetRasterizerState((ERasterizerState)RasterizerState);
}

void UMaterial::PostInitProperties()
{
    Super::PostInitProperties();

    SetRasterizerState((ERasterizerState)RasterizerState);
}

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

void UMaterial::SetRasterizerState(const ERasterizerState InRasterizerState)
{
    RasterizerState = InRasterizerState;

    switch (RasterizerState)
    {
    case ERasterizerState::E_SOLID_BACK:
        RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_CW>::GetRHI();
        break;
    case ERasterizerState::E_SOLID_FRONT:
        RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_CCW>::GetRHI();
        break;
    case ERasterizerState::E_SOLID_NONE:
        RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
        break;

    case ERasterizerState::E_WIREFRAME_BACK:
        RHIRasterizerState = TStaticRasterizerState<FM_Wireframe, CM_CW>::GetRHI();
        break;
    case ERasterizerState::E_WIREFRAME_FRONT:
        RHIRasterizerState = TStaticRasterizerState<FM_Wireframe, CM_CCW>::GetRHI();
        break;
    case ERasterizerState::E_WIREFRAME_NONE:
        RHIRasterizerState = TStaticRasterizerState<FM_Wireframe, CM_None>::GetRHI();
        break;
    default:
        _ASSERT(false);
        RHIRasterizerState = TStaticRasterizerState<FM_Solid, CM_None>::GetRHI();
        break;
    }
}
