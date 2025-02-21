#include "VertexFactory.h"

class FStaticMeshVertexDeclaration : public FVertexDeclaration
{
public:
    virtual void InitRHI(FRHICommandList& RHICmdList) override
    {
        FVertexDeclarationElementList Elements;
        Elements.push_back(FVertexElement(0, 0, VET_Float3, 0, sizeof(FVector3D)));
        VertexDeclarationRHI = GDynamicRHI->RHICreateVertexDeclaration(Elements);
    }
};
TGlobalResource<FStaticMeshVertexDeclaration> GStaticMeshVertexDeclaration;

void FVertexFactory::Create(TObjectPtr<FVertexBuffer> InVertexBuffer, TObjectPtr<FIndexBuffer> InIndexBuffer, const FConstantBufferInfo& ConstantBufferInfo)
{
	Declaration = GStaticMeshVertexDeclaration.VertexDeclarationRHI;
	VertexBuffer = InVertexBuffer;
	VertexBuffer->InitRHI(FRHICommandListExecutor::GetImmediateCommandList());

	IndexBuffer = InIndexBuffer;
	IndexBuffer->InitRHI(FRHICommandListExecutor::GetImmediateCommandList());

	UniformBuffer = ::RHICreateUniformBuffer(ConstantBufferInfo, &ObjectUniformBuffer, sizeof(ObjectUniformBuffer));
}

void FVertexFactory::UpdateObjectUniformBuffer(FRHICommandList& CommandList, const FObjectUniformBuffer& InBuffer)
{
	ObjectUniformBuffer = InBuffer;
	ObjectUniformBuffer.Matrix = ObjectUniformBuffer.Matrix.Transpose();
	RHIUpdateUniformBuffer(UniformBuffer, &ObjectUniformBuffer, sizeof(ObjectUniformBuffer));
	CommandList.SetShaderUniformBuffer(EShaderFrequency::SF_Vertex, UniformBuffer);
}
