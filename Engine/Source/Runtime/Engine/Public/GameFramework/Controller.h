#pragma once
#include "GameFramework/Actor.h"
#include "Controller.generated.h"

class ACharacter;
class APawn;

/**
 * 컨트롤러는 Pawn을 소유하여 그 행동을 제어할 수 있는 비물리적 액터입니다. PlayerController는 인간 플레이어가 Pawn을 제어하는 데 사용되고,
 * AIController는 제어하는 Pawn에 대한 인공지능을 구현합니다.
 * 컨트롤러는 Possess() 메서드를 사용하여 Pawn을 제어하며, UnPossess()를 호출하여 Pawn의 제어를 포기합니다.
 *
 * 컨트롤러는 제어하는 Pawn에서 발생하는 많은 이벤트에 대한 알림을 받습니다. 이를 통해 컨트롤러는 이 이벤트에 응답하여 동작을 구현하고, 이벤트를 가로채서 Pawn의 기본 동작을 대신 실행할 수 있습니다.
 *
 * ControlRotation (GetControlRotation()을 통해 액세스)은 제어하는 Pawn의 시야/조준 방향을 결정하며, 마우스나 게임패드와 같은 입력에 영향을 받습니다.
 *
 * @see https://docs.unrealengine.com/latest/INT/Gameplay/Framework/Controller/
 */
UCLASS(/*abstract, notplaceable, NotBlueprintable, HideCategories = (Collision, Rendering, Transformation), MinimalAPI*/)
class ENGINE_API AController : public AActor//, public INavAgentInterface
{
    GENERATED_BODY()

public:
};