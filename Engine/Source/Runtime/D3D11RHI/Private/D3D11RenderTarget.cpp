#include "D3D11RHIPrivate.h"

void FD3D11DynamicRHI::RHIBeginRenderPass(const FRHIRenderPassInfo& InInfo, const TCHAR* InName)
{
    FRHISetRenderTargetsInfo RTInfo;
    InInfo.ConvertToRenderTargetsInfo(RTInfo);
    SetRenderTargetsAndClear(RTInfo);

    RenderPassInfo = InInfo;

    // [깊이 반전(근평면 1.f)] 임시로 여기서 일괄 처리
    {
        // 반전된 깊이 스텐실 상태 설정
        D3D11_DEPTH_STENCIL_DESC invertedDepthStencilDesc;
        ZeroMemory(&invertedDepthStencilDesc, sizeof(invertedDepthStencilDesc));
        invertedDepthStencilDesc.DepthEnable = TRUE;
        invertedDepthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        invertedDepthStencilDesc.DepthFunc = D3D11_COMPARISON_GREATER; // 반전된 깊이 비교 함수

        TRefCountPtr<ID3D11DepthStencilState> invertedDepthStencilState;
        Direct3DDevice->CreateDepthStencilState(&invertedDepthStencilDesc, invertedDepthStencilState.GetInitReference());
        Direct3DDeviceIMContext->OMSetDepthStencilState(invertedDepthStencilState, 1);
    }

    //if (InInfo.NumOcclusionQueries > 0)
    //{
    //    RHIBeginOcclusionQueryBatch(InInfo.NumOcclusionQueries);
    //}
}

void FD3D11DynamicRHI::RHIEndRenderPass()
{
    //if (RenderPassInfo.NumOcclusionQueries > 0)
    //{
    //    RHIEndOcclusionQueryBatch();
    //}

    //UE::RHICore::ResolveRenderPassTargets(RenderPassInfo, [this](UE::RHICore::FResolveTextureInfo Info)
    //    {
    //        ResolveTexture(Info);
    //    });

    FRHIRenderTargetView RTV(nullptr, ERenderTargetLoadAction::ENoAction);
    FRHIDepthRenderTargetView DepthRTV(nullptr, ERenderTargetLoadAction::ENoAction, ERenderTargetStoreAction::ENoAction);
    SetRenderTargets(1, &RTV, &DepthRTV);
}