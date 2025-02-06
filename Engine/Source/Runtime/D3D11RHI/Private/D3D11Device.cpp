#include "D3D11RHIPrivate.h"

IMPLEMENT_MODULE(FD3D11DynamicRHIModule, D3D11RHI);

FD3D11DynamicRHI::FD3D11DynamicRHI(IDXGIFactory1* InDXGIFactory1, D3D_FEATURE_LEVEL InFeatureLevel, const FD3D11Adapter& InAdapter)
	: DXGIFactory1(InDXGIFactory1)
	, FeatureLevel(InFeatureLevel)
	, Adapter(InAdapter)
{
}

void FD3D11DynamicRHI::Shutdown()
{
}
