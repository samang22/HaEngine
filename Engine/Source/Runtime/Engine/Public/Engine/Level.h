#pragma once
#include "CoreMinimal.h"
#include "Level.generated.h"

//
// 레벨 객체. 레벨의 액터 목록, BSP 정보 및 브러시 목록을 포함합니다.
// 모든 레벨은 World를 Outer로 가지고 있으며 PersistentLevel로 사용할 수 있습니다.
// 그러나 레벨이 스트리밍되었을 때 OwningWorld는 그 레벨이 속한 World를 나타냅니다.
//

/**
 * 레벨은 액터(조명, 볼륨, 메쉬 인스턴스 등)의 모음입니다.
 * 여러 레벨을 월드에 로드하고 언로드하여 스트리밍 경험을 만들 수 있습니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Engine/Levels
 * @see UActor
 */
UCLASS()
class ENGINE_API ULevel : public UObject
{
	GENERATED_BODY()
public:
	ULevel();
};