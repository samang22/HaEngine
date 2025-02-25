#pragma once
#include "Components/MeshComponent.h"
#include "StaticMeshComponent.generated.h"

class UStaticMesh;

UCLASS()
class ENGINE_API UStaticMeshComponent : public UMeshComponent
{
	GENERATED_BODY()
public:
	UStaticMeshComponent();

	void SetStaticMesh(TEnginePtr<UStaticMesh> NewStaticMesh);

	TEnginePtr<UStaticMesh> GetStaticMesh() const { return StaticMesh; }

protected:
	virtual bool ShouldCreateRenderState() const;

private:
	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

private:
	UPROPERTY(EditAnywhere)
	TEnginePtr<UStaticMesh> StaticMesh;
};