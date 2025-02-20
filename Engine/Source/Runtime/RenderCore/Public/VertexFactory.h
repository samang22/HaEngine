#pragma once
#include "RenderResource.h"

/**
 * 정점 셰이더에 연결될 수 있는 정점 데이터 소스를 캡슐화합니다.
 */
class FVertexFactory : public FRenderResource
{
public:
    /** 일반적으로 팩토리를 렌더링하는 데 사용되는 RHI 정점 선언. */
    FVertexDeclarationRHIRef Declaration;

    TObjectPtr<FVertexBuffer> VertexBuffer;
    TObjectPtr<FIndexBuffer> IndexBuffer;
};