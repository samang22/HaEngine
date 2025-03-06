#pragma once
#include "CoreMinimal.h"
#include "VertexFactory.h"
#include "Materials/Material.h"
#include "StaticMesh.generated.h"

struct FMeshData;
class UMaterial;

struct FStaticMeshRenderData
{
public:
	void Create(class UStaticMesh* Outer, const FMeshData& NewMeshData);

	FString Name;

	FVertexFactory VertexFactory;

	TObjectPtr<UMaterial> Material;
	uint32 NumVertices = 0;
	uint32 NumPrimitives = 0;
};

UCLASS()
class ENGINE_API UStaticMesh : public UObject
{
    GENERATED_BODY()
public:
    UStaticMesh();
    virtual void Create(const TArray<FMeshData>& NewMeshData);
	TArray<FStaticMeshRenderData>& GetRenderData() { return RenderData; }
protected:
	TArray<FStaticMeshRenderData> RenderData;
};