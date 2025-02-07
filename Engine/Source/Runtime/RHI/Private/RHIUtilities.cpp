#include "RHIUtilities.h"
#include "RHIAccess.h"

RHI_API EGpuVendorId RHIGetPreferredAdapterVendor()
{
    /*if (FParse::Param(FCommandLine::Get(), TEXT("preferAMD")))
    {
        return EGpuVendorId::Amd;
    }

    if (FParse::Param(FCommandLine::Get(), TEXT("preferIntel")))
    {
        return EGpuVendorId::Intel;
    }

    if (FParse::Param(FCommandLine::Get(), TEXT("preferNvidia")))
    {
        return EGpuVendorId::Nvidia;
    }

    if (FParse::Param(FCommandLine::Get(), TEXT("preferMS")) || FParse::Param(FCommandLine::Get(), TEXT("preferMicrosoft")))
    {
        return EGpuVendorId::Microsoft;
    }*/

    return EGpuVendorId::Unknown;
}

ERHIAccess RHIGetDefaultResourceState(ETextureCreateFlags InUsage, bool bInHasInitialData)
{
	// By default assume it can be bound for reading
	ERHIAccess ResourceState = ERHIAccess::SRVMask;

	if (!bInHasInitialData)
	{
		if (EnumHasAnyFlags(InUsage, TexCreate_RenderTargetable))
		{
			ResourceState = ERHIAccess::RTV;
		}
		else if (EnumHasAnyFlags(InUsage, TexCreate_DepthStencilTargetable))
		{
			ResourceState = ERHIAccess::DSVWrite | ERHIAccess::DSVRead;
		}
		else if (EnumHasAnyFlags(InUsage, TexCreate_UAV))
		{
			ResourceState = ERHIAccess::UAVMask;
		}
		else if (EnumHasAnyFlags(InUsage, TexCreate_Presentable))
		{
			ResourceState = ERHIAccess::Present;
		}
		else if (EnumHasAnyFlags(InUsage, TexCreate_ShaderResource))
		{
			ResourceState = ERHIAccess::SRVMask;
		}
		else if (EnumHasAnyFlags(InUsage, TexCreate_Foveation))
		{
			ResourceState = ERHIAccess::ShadingRateSource;
		}
	}

	return ResourceState;
}