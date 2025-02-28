#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

UGameInstance::UGameInstance()
{
}

UEngine* UGameInstance::GetEngine() const
{
    return CastChecked<UEngine>(GetOuter()).get();
}

#if WITH_EDITOR
void UGameInstance::InitializeForPlayInEditor()
{
    UEngine* const EditorEngine = GetEngine();
    UWorld* EditorWorld = EditorEngine->GetEditorWorld();

    // 표준 PIE path: 에디터 월드를 단순히 복제합니다.
    TObjectPtr<UWorld> NewWorld = EditorEngine->CreatePIEWorldByDuplication(EditorWorld);

    EditorEngine->PlayWorld = NewWorld;
    NewWorld->SetGameInstance(this);
    World = NewWorld.get();

    // World 컨텍스트를 설정하고 게임 인스턴스를 초기화한 후 월드를 초기화하여 일반적인 로드와 일관되게 합니다.
    // 이 작업은 월드 서브시스템을 생성하고 게임 시작을 준비합니다.
    EditorEngine->PostCreatePIEWorld(NewWorld.get());
}

void UGameInstance::StartPlayInEditorGameInstance(ULocalPlayer* LocalPlayer)
{
    // 현재 월드에서 게임 플레이를 할 예정이므로, 플레이 준비를 합니다
    UWorld* const PlayWorld = GetWorld();

    // 생략
    //SpawnPlayFromHereStart
    {

    }

    // GameMode 생성 및 세팅
    // PlayWorld->SetGameMode
    {

    }

    // InitializeActorsForPlay
    {
        FRegisterComponentContext Context(PlayWorld);
        PlayWorld->InitializeActorsForPlay(&Context);
    }
}
#endif