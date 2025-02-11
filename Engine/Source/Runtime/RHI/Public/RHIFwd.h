#pragma once

#include "Templates/RefCounting.h"

class FRHITexture;
class FRHIViewport;

using FTextureRHIRef = TRefCountPtr<FRHITexture>;
using FViewportRHIRef = TRefCountPtr<FRHIViewport>;