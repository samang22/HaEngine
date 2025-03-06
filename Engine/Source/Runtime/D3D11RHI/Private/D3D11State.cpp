#include "D3D11RHIPrivate.h"
#include "D3D11State.h"

static D3D11_CULL_MODE TranslateCullMode(ERasterizerCullMode CullMode)
{
    switch (CullMode)
    {
    case CM_CW: return D3D11_CULL_BACK;
    case CM_CCW: return D3D11_CULL_FRONT;
    default: return D3D11_CULL_NONE;
    };
}

static D3D11_FILL_MODE TranslateFillMode(ERasterizerFillMode FillMode)
{
    switch (FillMode)
    {
    case FM_Wireframe: return D3D11_FILL_WIREFRAME;
    default: return D3D11_FILL_SOLID;
    };
}

FRasterizerStateRHIRef FD3D11DynamicRHI::RHICreateRasterizerState(const FRasterizerStateInitializerRHI& Initializer)
{
    D3D11_RASTERIZER_DESC RasterizerDesc = {};

    RasterizerDesc.CullMode = TranslateCullMode(Initializer.CullMode);
    RasterizerDesc.FillMode = TranslateFillMode(Initializer.FillMode);
    RasterizerDesc.SlopeScaledDepthBias = Initializer.SlopeScaleDepthBias;
    RasterizerDesc.FrontCounterClockwise = true;
    RasterizerDesc.DepthBias = (INT)floor(Initializer.DepthBias * (float)(1 << 24));
    RasterizerDesc.DepthClipEnable = Initializer.DepthClipMode == ERasterizerDepthClipMode::DepthClip;
    RasterizerDesc.MultisampleEnable = Initializer.bAllowMSAA;
    RasterizerDesc.ScissorEnable = true;

    FD3D11RasterizerState* RasterizerState = new FD3D11RasterizerState;
    VERIFYD3D11RESULT_EX(Direct3DDevice->CreateRasterizerState(&RasterizerDesc, RasterizerState->Resource.GetInitReference()), Direct3DDevice);
    return RasterizerState;
}