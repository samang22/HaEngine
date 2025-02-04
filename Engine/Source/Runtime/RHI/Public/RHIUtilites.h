#pragma once
#include "RHIDefinitions.h"

/** 선호하는 벤더의 VendorID를 반환하며, 지정되지 않은 경우 -1을 반환합니다. */
extern RHI_API EGpuVendorId RHIGetPreferredAdapterVendor();