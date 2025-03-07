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

static D3D11_COMPARISON_FUNC TranslateCompareFunction(ECompareFunction CompareFunction)
{
    switch (CompareFunction)
    {
    case CF_Less: return D3D11_COMPARISON_LESS;
    case CF_LessEqual: return D3D11_COMPARISON_LESS_EQUAL;
    case CF_Greater: return D3D11_COMPARISON_GREATER;
    case CF_GreaterEqual: return D3D11_COMPARISON_GREATER_EQUAL;
    case CF_Equal: return D3D11_COMPARISON_EQUAL;
    case CF_NotEqual: return D3D11_COMPARISON_NOT_EQUAL;
    case CF_Never: return D3D11_COMPARISON_NEVER;
    default: return D3D11_COMPARISON_ALWAYS;
    };
}

static D3D11_STENCIL_OP TranslateStencilOp(EStencilOp StencilOp)
{
    switch (StencilOp)
    {
    case SO_Zero: return D3D11_STENCIL_OP_ZERO;
    case SO_Replace: return D3D11_STENCIL_OP_REPLACE;
    case SO_SaturatedIncrement: return D3D11_STENCIL_OP_INCR_SAT;
    case SO_SaturatedDecrement: return D3D11_STENCIL_OP_DECR_SAT;
    case SO_Invert: return D3D11_STENCIL_OP_INVERT;
    case SO_Increment: return D3D11_STENCIL_OP_INCR;
    case SO_Decrement: return D3D11_STENCIL_OP_DECR;
    default: return D3D11_STENCIL_OP_KEEP;
    };
}

FDepthStencilStateRHIRef FD3D11DynamicRHI::RHICreateDepthStencilState(const FDepthStencilStateInitializerRHI& Initializer)
{
    FD3D11DepthStencilState* DepthStencilState = new FD3D11DepthStencilState;

    D3D11_DEPTH_STENCIL_DESC DepthStencilDesc = {};

    // depth part
    DepthStencilDesc.DepthEnable = Initializer.DepthTest != CF_Always || Initializer.bEnableDepthWrite;
    DepthStencilDesc.DepthWriteMask = Initializer.bEnableDepthWrite ? D3D11_DEPTH_WRITE_MASK_ALL : D3D11_DEPTH_WRITE_MASK_ZERO;
    DepthStencilDesc.DepthFunc = TranslateCompareFunction(Initializer.DepthTest);

    // stencil part
    DepthStencilDesc.StencilEnable = Initializer.bEnableFrontFaceStencil || Initializer.bEnableBackFaceStencil;
    DepthStencilDesc.StencilReadMask = Initializer.StencilReadMask;
    DepthStencilDesc.StencilWriteMask = Initializer.StencilWriteMask;
    DepthStencilDesc.FrontFace.StencilFunc = TranslateCompareFunction(Initializer.FrontFaceStencilTest);
    DepthStencilDesc.FrontFace.StencilFailOp = TranslateStencilOp(Initializer.FrontFaceStencilFailStencilOp);
    DepthStencilDesc.FrontFace.StencilDepthFailOp = TranslateStencilOp(Initializer.FrontFaceDepthFailStencilOp);
    DepthStencilDesc.FrontFace.StencilPassOp = TranslateStencilOp(Initializer.FrontFacePassStencilOp);
    if (Initializer.bEnableBackFaceStencil)
    {
        DepthStencilDesc.BackFace.StencilFunc = TranslateCompareFunction(Initializer.BackFaceStencilTest);
        DepthStencilDesc.BackFace.StencilFailOp = TranslateStencilOp(Initializer.BackFaceStencilFailStencilOp);
        DepthStencilDesc.BackFace.StencilDepthFailOp = TranslateStencilOp(Initializer.BackFaceDepthFailStencilOp);
        DepthStencilDesc.BackFace.StencilPassOp = TranslateStencilOp(Initializer.BackFacePassStencilOp);
    }
    else
    {
        DepthStencilDesc.BackFace = DepthStencilDesc.FrontFace;
    }

    const bool bStencilOpIsKeep =
        Initializer.FrontFaceStencilFailStencilOp == SO_Keep
        && Initializer.FrontFaceDepthFailStencilOp == SO_Keep
        && Initializer.FrontFacePassStencilOp == SO_Keep
        && Initializer.BackFaceStencilFailStencilOp == SO_Keep
        && Initializer.BackFaceDepthFailStencilOp == SO_Keep
        && Initializer.BackFacePassStencilOp == SO_Keep;

    const bool bMayWriteStencil = Initializer.StencilWriteMask != 0 && !bStencilOpIsKeep;
    DepthStencilState->AccessType.SetDepthStencilWrite(Initializer.bEnableDepthWrite, bMayWriteStencil);

    VERIFYD3D11RESULT_EX(Direct3DDevice->CreateDepthStencilState(&DepthStencilDesc, DepthStencilState->Resource.GetInitReference()), Direct3DDevice);
    return DepthStencilState;
}