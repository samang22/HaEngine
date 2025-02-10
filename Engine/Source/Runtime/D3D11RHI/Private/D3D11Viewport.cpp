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

