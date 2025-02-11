#pragma once

#include "Templates/RefCounting.h"

class FRHITexture;
class FRHIPixelShader;
class FRHIVertexShader;
class FRHIViewport;

using FTextureRHIRef = TRefCountPtr<FRHITexture>;
using FViewportRHIRef = TRefCountPtr<FRHIViewport>;
using FVertexShaderRHIRef = TRefCountPtr<FRHIVertexShader>;
using FPixelShaderRHIRef = TRefCountPtr<FRHIPixelShader>;
using FViewportRHIRef = TRefCountPtr<FRHIViewport>;