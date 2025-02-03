#include "D3D11RHIPrivate.h"


void FD3D11DynamicRHIModule::StartupModule()
{
}

bool FD3D11DynamicRHIModule::IsSupported()
{
    return true;
}

FDynamicRHI* FD3D11DynamicRHIModule::CreateRHI(ERHIFeatureLevel::Type RequestedFeatureLevel)
{
    return nullptr;
}

void FD3D11DynamicRHIModule::FindAdapter()
{
}