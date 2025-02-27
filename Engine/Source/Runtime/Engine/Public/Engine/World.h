#pragma once
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/EngineTypes.h"
#include "World.generated.h"

class ULevel;
class APawn;

struct ENGINE_API FActorSpawnParameters
{
	FActorSpawnParameters();
	/* 스폰된 액터의 이름으로 지정할 이름입니다. 값이 지정되지 않으면, 스폰된 액터의 이름은 [Class]_[Number] 형식으로 자동 생성됩니다. */
	FName Name;

	/* 새로운 액터를 스폰할 때 템플릿으로 사용할 액터입니다. 스폰된 액터는 템플릿 액터의 속성 값을 사용하여 초기화됩니다. 만약 이 값이 NULL로 남아있으면, 클래스 기본 객체 (CDO)를 사용하여 스폰된 액터를 초기화합니다. */
	AActor* Template;

	/* 이 액터를 스폰한 액터입니다. (NULL로 남겨둘 수 있습니다). */
	AActor* Owner;

	/* 스폰된 액터가 입힌 피해에 책임이 있는 APawn입니다. (NULL로 남겨둘 수 있습니다). */
	APawn* Instigator;

	/** 제공된 스폰 Transform을 사용하여 루트 컴포넌트를 곱할지 무시할지 결정합니다 */
	ESpawnActorScaleMethod TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;

	/* 스폰된 액터/객체 인스턴스를 설명하는 데 사용되는 플래그입니다. */
	EObjectFlags ObjectFlags;
};

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
	UWorld();
	~UWorld();

public:
	/**
	 * 새로 생성된 월드를 초기화합니다.
	 */
	void InitalizeNewWorld();
	/**
	 * 월드를 초기화하고, 지속적인 레벨을 연결하며, 적절한 영역을 설정합니다.
	 */
	void InitWorld();
	void Tick(float DeltaSeconds);

public:
	virtual void Serialize(FArchive& Ar) override;
	string Save();
	void Load(const string& InLoadString);

public:
	/**
	 * 에디터 월드를 복제하여 PIE 월드를 생성합니다.
	 */
	static TObjectPtr<UWorld> GetDuplicatedWorldForPIE(UWorld* InWorld);

public:
	/**
	 * 주어진 Transform과 스폰 파라미터를 사용하여 액터를 스폰합니다.
	 *
	 * @param   Class                   스폰할 클래스
	 * @param   Transform               스폰할 월드 변환
	 * @param   SpawnParameters         스폰 파라미터
	 *
	 * @return  방금 스폰된 액터
	*/
	AActor* SpawnActor(UClass* Class, FTransform const* Transform, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters());

	/**
	 *  SpawnActor의 템플릿 버전입니다.
	*/
	template<class T>
	T* SpawnActor(UClass* Class, FTransform const& Transform, const FActorSpawnParameters& SpawnParameters = FActorSpawnParameters())
	{
		if (Class == nullptr)
		{
			Class = T::StaticClass();
		}
		return CastCheckedRaw<T>(SpawnActor(Class, &Transform, SpawnParameters));
	}

public:
	/** 액터가 초기화되고 게임을 시작할 준비가 되면 true를 반환합니다. */
	bool AreActorsInitialized() const;

public:
	/** 이 월드가 게임 월드(PIE 월드 포함)인지 여부를 반환합니다. */
	bool IsGameWorld() const;

	/** 이 월드가 에디터 월드(에디터 미리보기 월드 포함)인지 여부를 반환합니다. */
	bool IsEditorWorld() const;

	/** 이 월드의 타입입니다. 사용되는 Context을 설명합니다(에디터, 게임, 프리뷰 등). */
	EWorldType::Type WorldType = EWorldType::Type::None;

private:
	/** 월드 정보, 기본 브러시 및 게임 플레이 중 스폰된 액터 등을 포함하는 PersistentLevel */
	shared_ptr<ULevel> PersistentLevel;
	FTransform Transform;

public:
	/**
	 * 액터가 스폰될 때마다 알림을 브로드캐스트합니다.
	 * 이 이벤트는 새로 생성된 액터에만 해당됩니다.
	 */
	FDelegate<AActor*> OnActorSpawned;

public:
	/** 이 월드의 씬 매니저에 대한 인터페이스. */
	class FSceneInterface* Scene = nullptr;

	/** 액터가 게임을 위해 초기화되었는지 여부 */
	uint8 bActorsInitialized : 1 = false;

};
extern ENGINE_API FDelegate<UWorld*> WorldCreatedDelegate;
extern ENGINE_API FDelegate<UWorld*> WorldDestroyedDelegate;