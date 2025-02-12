#pragma once

#include "Templates/RefCounting.h"

class FRHITexture;
class FRHIPixelShader;
class FRHIVertexShader;
class FRHIViewport;
class FRHIVertexDeclaration;

using FTextureRHIRef = TRefCountPtr<FRHITexture>;
using FViewportRHIRef = TRefCountPtr<FRHIViewport>;
using FVertexShaderRHIRef = TRefCountPtr<FRHIVertexShader>;
using FPixelShaderRHIRef = TRefCountPtr<FRHIPixelShader>;
using FViewportRHIRef = TRefCountPtr<FRHIViewport>;
using FVertexDeclarationRHIRef = TRefCountPtr<FRHIVertexDeclaration>;

struct FVertexElement;
typedef TArray<FVertexElement> FVertexDeclarationElementList;