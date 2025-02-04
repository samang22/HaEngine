#include "RHIUtilites.h"

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