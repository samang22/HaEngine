#pragma once
#include "Components/SceneComponent.h"
#include "PrimitiveComponent.generated.h"

/**
 * PrimitiveComponent는 렌더링되거나 충돌 데이터로 사용하기 위해 일정한 형태의 지오메트리를 포함하거나 생성하는 SceneComponent입니다.
 * 여러 유형의 지오메트리를 위한 여러 서브클래스가 있지만, 가장 일반적인 것은 ShapeComponent(캡슐, 구, 박스), StaticMeshComponent 및 SkeletalMeshComponent입니다.
 * ShapeComponent는 충돌 감지를 위해 사용되는 지오메트리를 생성하지만 렌더링되지 않으며, StaticMeshComponent와 SkeletalMeshComponent는 렌더링되는 사전 제작된 지오메트리를 포함하지만 충돌 감지에도 사용할 수 있습니다.
 */
UCLASS()
class ENGINE_API UPrimitiveComponent : public USceneComponent
{
	GENERATED_BODY()
public:
	UPrimitiveComponent();
};