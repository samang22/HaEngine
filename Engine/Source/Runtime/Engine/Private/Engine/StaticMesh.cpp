#include "Engine/StaticMesh.h"
#include "Factories/FbxFactory.h"

class ENGINE_API FStaticMeshVertexDeclaration : public FVertexDeclaration
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

class ENGINE_API FStaticMeshVertexBuffer : public FVertexBuffer
{
public:
    TResourceArray<FVector3D> VertexData;

    virtual void InitRHI(FRHICommandList& RHICmdList) override
    {
        FRHIResourceCreateInfo CreateInfo(TEXT("VertexBuffer"), &VertexData);
        VertexBufferRHI = GetCommandList().CreateVertexBuffer(VertexData.GetResourceDataSize(), BUF_Static, CreateInfo);
        if (!VertexBufferRHI)
        {
            E_LOG(Warning, TEXT("VertexBufferRHI creation failed"));
            return;
        }
    }
};

class ENGINE_API FStaticMeshIndexBuffer : public FIndexBuffer
{
public:
    TResourceArray<uint32> IndexData;

    virtual void InitRHI(FRHICommandList& RHICmdList) override
    {
        FRHIResourceCreateInfo CreateInfo(TEXT("IndexBuffer"), &IndexData);
        IndexBufferRHI = GetCommandList().CreateIndexBuffer(IndexData.GetResourceDataSize(), IndexData.GetTypeSize(), BUF_Static, CreateInfo);        
        if (!IndexBufferRHI)
        {
            E_LOG(Warning, TEXT("IndexBufferRHI creation failed"));
            return;
        }
    }

};
UStaticMesh::UStaticMesh()
{

}

void FStaticMeshRenderData::Create(UStaticMesh* Outer, const FMeshData& NewMeshData)
{
    TObjectPtr<FStaticMeshVertexBuffer> VertexBuffer = make_shared<FStaticMeshVertexBuffer>();
    VertexBuffer->VertexData = NewMeshData.Vertices;

    TObjectPtr<FStaticMeshIndexBuffer> IndexBuffer = make_shared<FStaticMeshIndexBuffer>();
    IndexBuffer->IndexData = NewMeshData.Indices;

    VertexFactory.Create(VertexBuffer, IndexBuffer);

    NumVertices = NewMeshData.Vertices.size();
    NumPrimitives = NewMeshData.NumPrimitives;

    TShaderMapRef<FMaterialVS> VertexShader;
    TShaderMapRef<FMaterialPS> PixelShader;
    Material = NewObject<UMaterial>(Outer, UMaterial::StaticClass());
    Material->SetVertexShader(VertexShader);
    Material->SetPixelShader(PixelShader);
}

void UStaticMesh::Create(const TArray<FMeshData>& NewMeshData)
{
    RenderData.resize(NewMeshData.size());
    for (uint32 i = 0; i < RenderData.size(); ++i)
    {
        RenderData[i].Create(this, NewMeshData[i]);
    }
} 