#pragma once
#include "Components/MeshComponent.h"
#include "StaticMeshComponent.generated.h"

class UStaticMesh;
class UMaterial;

UCLASS()
class ENGINE_API UStaticMeshComponent : public UMeshComponent
{
	GENERATED_BODY()
public:
	UStaticMeshComponent();

	void SetStaticMesh(TEnginePtr<UStaticMesh> NewStaticMesh);

	TEnginePtr<UStaticMesh> GetStaticMesh() const { return StaticMesh; }
	TEnginePtr<UMaterial> GetMaterial(int32 MaterialIndex);
	TArray<TObjectPtr<UMaterial>>& GetOverrideMaterials() { return OverrideMaterials; }
	void SetMaterial(TObjectPtr<UMaterial> NewMaterial, int32 MaterialIndex);
	uint32 GetMaterialCount();

protected:
	virtual bool ShouldCreateRenderState() const;

private:
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

private:
	UPROPERTY(EditAnywhere)
	TEnginePtr<UStaticMesh> StaticMesh;
	TArray<TObjectPtr<UMaterial>> OverrideMaterials;
};