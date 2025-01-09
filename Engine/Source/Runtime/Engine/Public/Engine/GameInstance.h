#pragma once
#include "CoreMinimal.h"
#include "GameInstance.generated.h"

/**
 * GameInstance: 실행 중인 게임의 인스턴스를 위한 고수준 관리자 객체.
 * 게임 생성 시 생성되며, 게임 인스턴스가 종료될 때까지 파괴되지 않습니다.
 * 독립 실행형 게임으로 실행 중인 경우, 하나의 GameInstance가 존재합니다.
 * PIE(에디터 내 플레이)에서 실행 중인 경우, PIE 인스턴스당 하나의 GameInstance가 생성됩니다.
 */

UCLASS()
class ENGINE_API UGameInstance : public UObject
{
    GENERATED_BODY()
public:
    UGameInstance();
};