#include "D3D11RHIPrivate.h"
#include "RHIResources.h"
#include "D3D11Viewport.h"

FViewportRHIRef FD3D11DynamicRHI::RHICreateViewport(void* WindowHandle, uint32 SizeX, uint32 SizeY, bool bIsFullscreen, EPixelFormat PreferredPixelFormat)
{
    if (PreferredPixelFormat != EPixelFormat::PF_A2B10G10R10)
    {
        _ASSERT(false);
    }
    return new FD3D11Viewport(this, (HWND)WindowHandle, SizeX, SizeY, bIsFullscreen, PreferredPixelFormat);
}

void FD3D11DynamicRHI::RHIBeginDrawingViewport(FRHIViewport* ViewportRHI, FRHITexture* RenderTarget)
{
	FD3D11Viewport* Viewport = ResourceCast(ViewportRHI);
	_ASSERT(!DrawingViewport);
	DrawingViewport = Viewport;

	// 렌더 타겟과 뷰포트를 설정합니다.
	if (RenderTarget == NULL)
	{
		RenderTarget = Viewport->GetBackBuffer();
	}

	//FRHIRenderTargetView View(RenderTarget, ERenderTargetLoadAction::ELoad);
	//SetRenderTargets(1, &View, nullptr);


	FRHISetRenderTargetsInfo RTInfo;
	FRHIRenderPassInfo RPInfo(RenderTarget, ERenderTargetActions::Clear_DontStore);
	RPInfo.ConvertToRenderTargetsInfo(RTInfo);
	SetRenderTargetsAndClear(RTInfo);

	// 우리는 생략 가능
	// RHISetScissorRect

	FVector3D RenderTargetSize = RenderTarget->GetSizeXY();
	const D3D11_VIEWPORT Viewports[1] = { 0.f, 0.f, RenderTargetSize.x, RenderTargetSize.y, 0.f, 1.f };
	Direct3DDeviceIMContext->RSSetViewports(1, Viewports);
}

void FD3D11DynamicRHI::RHIEndDrawingViewport(FRHIViewport* ViewportRHI, bool bPresent, bool bLockToVsync)
{
	FD3D11Viewport* Viewport = ResourceCast(ViewportRHI);

	_ASSERT(DrawingViewport.GetReference() == Viewport);
	DrawingViewport = NULL;

	// Clear references the device might have to resources.
	CurrentDepthTexture = NULL;
	CurrentDepthStencilTarget = NULL;
	CurrentDSVAccessType = FExclusiveDepthStencil::DepthWrite_StencilWrite;
	CurrentRenderTargets[0] = NULL;
	for (uint32 RenderTargetIndex = 1; RenderTargetIndex < D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT; ++RenderTargetIndex)
	{
		CurrentRenderTargets[RenderTargetIndex] = NULL;
	}

	//ClearAllShaderResources();

	//CommitRenderTargetsAndUAVs();
	
	//StateCache.SetVertexShader(nullptr);
	
	//uint16 NullStreamStrides[MaxVertexElementCount] = { 0 };
	//StateCache.SetStreamStrides(NullStreamStrides);
	//for (uint32 StreamIndex = 0; StreamIndex < MaxVertexElementCount; ++StreamIndex)
	//{
	//	StateCache.SetStreamSource(nullptr, StreamIndex, 0, 0);
	//}
	
	//StateCache.SetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
	
	//CurrentResourceBoundAsIB = nullptr;
	//FMemory::Memzero(CurrentResourcesBoundAsVBs, sizeof(CurrentResourcesBoundAsVBs));
	//MaxBoundVertexBufferIndex = INDEX_NONE;
	
	//StateCache.SetPixelShader(nullptr);
	//StateCache.SetGeometryShader(nullptr);
	//// Compute Shader is set to NULL after each Dispatch call, so no need to clear it here


	bool bNativelyPresented = true;
	if (bPresent)
	{
		bNativelyPresented = Viewport->Present(bLockToVsync);
	}
}

void FD3D11DynamicRHI::RHIClearMRTImpl(const bool* bClearColorArray, int32 NumClearColors, const FLinearColor* ClearColorArray, bool bClearDepth, float Depth, bool bClearStencil, uint32 Stencil)
{
	FD3D11BoundRenderTargets BoundRenderTargets(Direct3DDeviceIMContext);

	// 모든 활성 렌더 타겟에 대해 충분한 클리어 색을 지정해야 합니다.
	_ASSERT(!bClearColorArray || NumClearColors >= BoundRenderTargets.GetNumActiveTargets());

	// 깊이나 스텐실을 클리어해야 하고 읽기 전용 깊이/스텐실 뷰가 바인딩되어 있는 경우, 쓰기 가능한 깊이/스텐실 뷰를 사용해야 합니다.
	if (CurrentDepthTexture)
	{
		FExclusiveDepthStencil RequestedAccess;

		RequestedAccess.SetDepthStencilWrite(bClearDepth, bClearStencil);

		_ASSERT(RequestedAccess.IsValid(CurrentDSVAccessType));
	}

	ID3D11DepthStencilView* DepthStencilView = BoundRenderTargets.GetDepthStencilView();

	if (bClearColorArray && BoundRenderTargets.GetNumActiveTargets() > 0)
	{
		for (int32 TargetIndex = 0; TargetIndex < BoundRenderTargets.GetNumActiveTargets(); TargetIndex++)
		{
			if (bClearColorArray[TargetIndex])
			{
				ID3D11RenderTargetView* RenderTargetView = BoundRenderTargets.GetRenderTargetView(TargetIndex);
				if (RenderTargetView != nullptr)
				{
					Direct3DDeviceIMContext->ClearRenderTargetView(RenderTargetView, (float*)&ClearColorArray[TargetIndex]);
				}
			}
		}
	}

	if ((bClearDepth || bClearStencil) && DepthStencilView)
	{
		uint32 ClearFlags = 0;
		if (bClearDepth)
		{
			ClearFlags |= D3D11_CLEAR_DEPTH;
		}
		if (bClearStencil)
		{
			ClearFlags |= D3D11_CLEAR_STENCIL;
		}
		Direct3DDeviceIMContext->ClearDepthStencilView(DepthStencilView, ClearFlags, Depth, Stencil);
	}
}

void FD3D11DynamicRHI::SetRenderTargets(
	uint32 NewNumSimultaneousRenderTargets,
	const FRHIRenderTargetView* NewRenderTargetsRHI,
	const FRHIDepthRenderTargetView* NewDepthStencilTargetRHI)
{
	FD3D11Texture* NewDepthStencilTarget = ResourceCast(NewDepthStencilTargetRHI ? NewDepthStencilTargetRHI->Texture : nullptr);

	_ASSERT(NewNumSimultaneousRenderTargets <= MaxSimultaneousRenderTargets);

	bool bTargetChanged = false;

	// 깊이 쓰기가 활성화되어 있는지 여부에 따라 적절한 깊이 스텐실 뷰를 설정합니다.
	ID3D11DepthStencilView* DepthStencilView = NULL;
	if (NewDepthStencilTarget)
	{
		_ASSERT(NewDepthStencilTargetRHI);
		CurrentDSVAccessType = NewDepthStencilTargetRHI->GetDepthStencilAccess();
		DepthStencilView = NewDepthStencilTarget->GetDepthStencilView(CurrentDSVAccessType);

		// 바인딩된 깊이 스텐실 타겟의 셰이더 뷰를 언바인딩합니다.
		ConditionalClearShaderResource(NewDepthStencilTarget, false);
	}

	// 깊이 스텐실 타겟이 이전 상태와 다른지 확인합니다.
	if (CurrentDepthStencilTarget != DepthStencilView)
	{
		CurrentDepthTexture = NewDepthStencilTarget;
		CurrentDepthStencilTarget = DepthStencilView;
		bTargetChanged = true;
	}

	// 새로운 렌더 타겟들을 위한 렌더 타겟 뷰를 모읍니다.
	ID3D11RenderTargetView* NewRenderTargetViews[MaxSimultaneousRenderTargets];
	for (uint32 RenderTargetIndex = 0; RenderTargetIndex < MaxSimultaneousRenderTargets; ++RenderTargetIndex)
	{
		ID3D11RenderTargetView* RenderTargetView = NULL;
		if (RenderTargetIndex < NewNumSimultaneousRenderTargets && NewRenderTargetsRHI[RenderTargetIndex].Texture != nullptr)
		{
			int32 RTMipIndex = NewRenderTargetsRHI[RenderTargetIndex].MipIndex;
			int32 RTSliceIndex = NewRenderTargetsRHI[RenderTargetIndex].ArraySliceIndex;

			FD3D11Texture* NewRenderTarget = ResourceCast(NewRenderTargetsRHI[RenderTargetIndex].Texture);
			RenderTargetView = NewRenderTarget ? NewRenderTarget->GetRenderTargetView(RTMipIndex, RTSliceIndex) : nullptr;

			_ASSERT(RenderTargetView, TEXT("Texture being set as render target has no RTV"));

			// 바인딩된 렌더 타겟의 셰이더 뷰를 언바인딩합니다.
			ConditionalClearShaderResource(NewRenderTarget, false);

#if UE_BUILD_DEBUG	
			// A check to allow you to pinpoint what is using mismatching targets
			// We filter our d3ddebug spew that checks for this as the d3d runtime's check is wrong.
			// For filter code, see D3D11Device.cpp look for "OMSETRENDERTARGETS_INVALIDVIEW"
			if (RenderTargetView && DepthStencilView)
			{
				FRTVDesc RTTDesc = GetRenderTargetViewDesc(RenderTargetView);

				TRefCountPtr<ID3D11Texture2D> DepthTargetTexture;
				DepthStencilView->GetResource((ID3D11Resource**)DepthTargetTexture.GetInitReference());

				D3D11_TEXTURE2D_DESC DTTDesc;
				DepthTargetTexture->GetDesc(&DTTDesc);

				// enforce color target is <= depth and MSAA settings match
				if (RTTDesc.Width > DTTDesc.Width || RTTDesc.Height > DTTDesc.Height ||
					RTTDesc.SampleDesc.Count != DTTDesc.SampleDesc.Count ||
					RTTDesc.SampleDesc.Quality != DTTDesc.SampleDesc.Quality)
				{
					UE_LOG(LogD3D11RHI, Fatal, TEXT("RTV(%i,%i c=%i,q=%i) and DSV(%i,%i c=%i,q=%i) have mismatching dimensions and/or MSAA levels!"),
						RTTDesc.Width, RTTDesc.Height, RTTDesc.SampleDesc.Count, RTTDesc.SampleDesc.Quality,
						DTTDesc.Width, DTTDesc.Height, DTTDesc.SampleDesc.Count, DTTDesc.SampleDesc.Quality);
				}
			}
#endif
		}

		NewRenderTargetViews[RenderTargetIndex] = RenderTargetView;

		// Check if the render target is different from the old state.
		if (CurrentRenderTargets[RenderTargetIndex] != RenderTargetView)
		{
			CurrentRenderTargets[RenderTargetIndex] = RenderTargetView;
			bTargetChanged = true;
		}
	}
	if (NumSimultaneousRenderTargets != NewNumSimultaneousRenderTargets)
	{
		NumSimultaneousRenderTargets = NewNumSimultaneousRenderTargets;
		uint32 Bit = 1;
		uint32 Mask = 0;
		for (uint32 Index = 0; Index < NumSimultaneousRenderTargets; ++Index)
		{
			Mask |= Bit;
			Bit <<= 1;
		}
		CurrentRTVOverlapMask = Mask;
		bTargetChanged = true;
	}

	// 실제로 무언가가 변경된 경우에만 렌더 타겟을 변경하는 D3D 호출을 수행합니다.
	if (bTargetChanged)
	{
		CommitRenderTargets(true);
		CurrentUAVMask = 0;
	}

	// 뷰포트를 렌더 타겟 0의 전체 크기로 설정합니다.
	//if (NewRenderTargetViews[0])
	//{
	//	// check target 0 is valid
	//	check(0 < NewNumSimultaneousRenderTargets && NewRenderTargetsRHI[0].Texture != nullptr);
	//	FRTVDesc RTTDesc = GetRenderTargetViewDesc(NewRenderTargetViews[0]);
	//	RHISetViewport(0.0f, 0.0f, 0.0f, (float)RTTDesc.Width, (float)RTTDesc.Height, 1.0f);
	//}
	//else if (DepthStencilView)
	//{
	//	TRefCountPtr<ID3D11Texture2D> DepthTargetTexture;
	//	DepthStencilView->GetResource((ID3D11Resource**)DepthTargetTexture.GetInitReference());

	//	D3D11_TEXTURE2D_DESC DTTDesc;
	//	DepthTargetTexture->GetDesc(&DTTDesc);
	//	RHISetViewport(0.0f, 0.0f, 0.0f, (float)DTTDesc.Width, (float)DTTDesc.Height, 1.0f);
	//}
}

void FD3D11DynamicRHI::SetRenderTargetsAndClear(const FRHISetRenderTargetsInfo& RenderTargetsInfo)
{
	this->SetRenderTargets(RenderTargetsInfo.NumColorRenderTargets,
		RenderTargetsInfo.ColorRenderTarget,
		&RenderTargetsInfo.DepthStencilRenderTarget);

	if (RenderTargetsInfo.bClearColor || RenderTargetsInfo.bClearStencil || RenderTargetsInfo.bClearDepth)
	{
		FLinearColor ClearColors[MaxSimultaneousRenderTargets];
		bool bClearColorArray[MaxSimultaneousRenderTargets];
		float DepthClear = 0.0;
		uint32 StencilClear = 0;

		if (RenderTargetsInfo.bClearColor)
		{
			for (int32 i = 0; i < RenderTargetsInfo.NumColorRenderTargets; ++i)
			{
				bClearColorArray[i] = RenderTargetsInfo.ColorRenderTarget[i].LoadAction == ERenderTargetLoadAction::EClear;

				if (bClearColorArray[i] && RenderTargetsInfo.ColorRenderTarget[i].Texture != nullptr)
				{
					const FClearValueBinding& ClearValue = RenderTargetsInfo.ColorRenderTarget[i].Texture->GetClearBinding();
					_ASSERT(ClearValue.ColorBinding == EClearBinding::EColorBound, TEXT("Texture: %s does not have a color bound for fast clears"), *RenderTargetsInfo.ColorRenderTarget[i].Texture->GetName().GetPlainNameString());
					ClearColors[i] = ClearValue.GetClearColor();
				}
			}
		}
		if (RenderTargetsInfo.bClearDepth || RenderTargetsInfo.bClearStencil)
		{
			const FClearValueBinding& ClearValue = RenderTargetsInfo.DepthStencilRenderTarget.Texture->GetClearBinding();
			_ASSERT(ClearValue.ColorBinding == EClearBinding::EDepthStencilBound, TEXT("Texture: %s does not have a DS value bound for fast clears"), *RenderTargetsInfo.DepthStencilRenderTarget.Texture->GetName().GetPlainNameString());
			ClearValue.GetDepthStencil(DepthClear, StencilClear);
		}

		this->RHIClearMRTImpl(RenderTargetsInfo.bClearColor ? bClearColorArray : nullptr, RenderTargetsInfo.NumColorRenderTargets, ClearColors, RenderTargetsInfo.bClearDepth, DepthClear, RenderTargetsInfo.bClearStencil, StencilClear);
	}
}

void FD3D11DynamicRHI::ConditionalClearShaderResource(FD3D11ViewableResource* Resource, bool bCheckBoundInputAssembler)
{
	_ASSERT(Resource);
	//ClearShaderResourceViews<SF_Vertex>(Resource);
	//ClearShaderResourceViews<SF_Pixel>(Resource);
	//ClearShaderResourceViews<SF_Geometry>(Resource);
	//ClearShaderResourceViews<SF_Compute>(Resource);

	//if (bCheckBoundInputAssembler)
	//{
	//	for (int32 ResourceIndex = MaxBoundVertexBufferIndex; ResourceIndex >= 0; --ResourceIndex)
	//	{
	//		if (CurrentResourcesBoundAsVBs[ResourceIndex] == Resource)
	//		{
	//			// Unset the vertex buffer from the device context
	//			TrackResourceBoundAsVB(nullptr, ResourceIndex);
	//			StateCache.SetStreamSource(nullptr, ResourceIndex, 0);
	//		}
	//	}

	//	if (Resource == CurrentResourceBoundAsIB)
	//	{
	//		TrackResourceBoundAsIB(nullptr);
	//		StateCache.SetIndexBuffer(nullptr, DXGI_FORMAT_R16_UINT, 0);
	//	}
	//}
}

void FD3D11DynamicRHI::CommitRenderTargets(bool bClearUAVS)
{
	ID3D11RenderTargetView* RTArray[D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT];
	for (uint32 RenderTargetIndex = 0; RenderTargetIndex < NumSimultaneousRenderTargets; ++RenderTargetIndex)
	{
		RTArray[RenderTargetIndex] = CurrentRenderTargets[RenderTargetIndex];
	}


	Direct3DDeviceIMContext->OMSetRenderTargets(
		NumSimultaneousRenderTargets,
		RTArray,
		CurrentDepthStencilTarget
	);

	/*if (bClearUAVs)
	{
		for (uint32 i = 0; i < D3D11_PS_CS_UAV_REGISTER_COUNT; ++i)
		{
			CurrentUAVs[i] = nullptr;
			UAVBound[i] = nullptr;
		}
		UAVBindFirst = 0;
		UAVBindCount = 0;
		UAVSChanged = 0;
	}*/
}
