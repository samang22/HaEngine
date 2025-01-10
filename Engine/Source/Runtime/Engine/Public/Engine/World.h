#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "World.generated.h"

class ULevel;
/**
 * World는 액터와 컴포넌트가 존재하고 렌더링되는 맵 또는 샌드박스를 나타내는 최상위 객체입니다.
 *
 * World는 단일 Persistent Level과 볼륨 및 블루프린트 함수로 로드되고 언로드되는 선택적 스트리밍 레벨 목록을 포함할 수 있으며,
 * 또는 World Composition으로 구성된 레벨 모음일 수 있습니다.
 *
 * 독립 실행형 게임에서는 일반적으로 하나의 World만 존재합니다. 단, 매끄러운 영역 전환 동안에는 목적지와 현재 월드가 모두 존재할 수 있습니다.
 * 에디터에서는 여러 World가 존재합니다: 편집 중인 레벨, 각 PIE 인스턴스, 상호작용 렌더 뷰포트를 가진 각 에디터 도구 등 다양한 경우가 있습니다.
 *
 */
UCLASS()
class ENGINE_API UWorld : public UObject
{
	GENERATED_BODY()

public:
	void InitalizeNewWorld();

public:
	/** 이 월드의 타입입니다. 사용되는 Context을 설명합니다(에디터, 게임, 프리뷰 등). */
	EWorldType::Type WorldType = EWorldType::Type::None;

private:
	/** 월드 정보, 기본 브러시 및 게임 플레이 중 스폰된 액터 등을 포함하는 PersistentLevel */
	shared_ptr<ULevel> PersistentLevel;
};