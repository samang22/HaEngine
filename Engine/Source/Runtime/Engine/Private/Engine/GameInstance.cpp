#include "Engine/GameInstance.h"
#include "Engine/Engine.h"

UGameInstance::UGameInstance()
{
}

UEngine* UGameInstance::GetEngine() const
{
    return CastChecked<UEngine>(GetOuter()).get();
}

void UGameInstance::InitializeForPlayInEditor()
{
    UEngine* const EditorEngine = GetEngine();
    UWorld* EditorWorld = EditorEngine->GetEditorWorld();

    // 표준 PIE path: 에디터 월드를 단순히 복제합니다.
    TObjectPtr<UWorld> NewWorld = EditorEngine->CreatePIEWorldByDuplication(EditorWorld);
}
