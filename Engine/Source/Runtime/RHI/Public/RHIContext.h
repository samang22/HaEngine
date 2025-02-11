#pragma once

class FRHIViewport;
class FRHITexture;

// RHI 명령 컨텍스트 인터페이스. 때로는 RHI가 이를 처리합니다.
// 명령 리스트를 병렬로 처리할 수 있는 플랫폼에서는 별도의 객체입니다.
class IRHICommandContext
{
public:
	virtual ~IRHICommandContext()
	{
	}


    // 이 메서드는 RHIThread와 함께 대기열에 추가되며, 그렇지 않으면 대기열에 추가된 후 플러시됩니다. 
    // RHI 스레드가 없으면 이 프레임 전진 명령을 대기열에 추가하는 데 이점이 없습니다.
    virtual void RHIBeginDrawingViewport(FRHIViewport* Viewport, FRHITexture* RenderTargetRHI) = 0;

    // 이 메서드는 RHIThread와 함께 대기열에 추가되며, 그렇지 않으면 대기열에 추가된 후 플러시됩니다.
    // RHI 스레드가 없으면 이 프레임 전진 명령을 대기열에 추가하는 데 이점이 없습니다.
    virtual void RHIEndDrawingViewport(FRHIViewport* Viewport, bool bPresent, bool bLockToVsync) = 0;

    virtual void RHIClearMRTImpl(const bool* bClearColorArray, int32 NumClearColors, const FLinearColor* ColorArray, bool bClearDepth, float Depth, bool bClearStencil, uint32 Stencil) = 0;
};