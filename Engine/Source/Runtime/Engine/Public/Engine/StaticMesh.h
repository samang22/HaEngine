#pragma once
#include "CoreMinimal.h"
#include "StaticMesh.generated.h"

struct FMeshData;
class FStaticMeshVertexBuffer;
class FStaticMeshIndexBuffer;

UCLASS()
class ENGINE_API UStaticMesh : public UObject
{
    GENERATED_BODY()
public:
    UStaticMesh();
    virtual void Create(const TArray<FMeshData>& NewMeshData);

protected:
	struct FRenderData
	{
	public:
		void Create(const FMeshData& NewMeshData);
		TObjectPtr<FStaticMeshVertexBuffer> VertexBuffer;
		TObjectPtr<FStaticMeshIndexBuffer> IndexBuffer;
		// Material
		uint32 NumVertices = 0;
		uint32 NumPrimitives = 0;
	};
	TArray<FRenderData> RenderData;
};