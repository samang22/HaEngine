#pragma once
#include "CoreMinimal.h"

inline uint32 GetMax2DTextureDimension()
{
    return 4096;
    //return GRHIGlobals.MaxTextureDimensions;
}

inline uint32 GetMaxCubeTextureDimension()
{
    return 2048;
    //return GRHIGlobals.MaxTextureDimensions;
}