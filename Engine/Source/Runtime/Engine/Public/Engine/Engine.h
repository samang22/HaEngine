#pragma once
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Engine.generated.h"

class UEditorViewportClient;
class UViewportClient;

UCLASS()
class ENGINE_API UEngine : public UObject
{
    GENERATED_BODY()
public:
    UEngine();
    void Init(HWND hViewportHandle);
    void Tick(float DeltaSeconds);
    void PreExit();

    void WndProc(UINT Message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:
    HWND MainViewportHandle = NULL;
    shared_ptr<UWorld> World;
    shared_ptr<UWorld> EditorWorld;

    TObjectPtr<UEditorViewportClient> EditorViewportClient;
    TEnginePtr<UViewportClient> CurrentViewportClient;
};

extern ENGINE_API UWorld* GWorld;

/** Global engine pointer. Can be 0 so don't use without checking. */
extern ENGINE_API TObjectPtr<UEngine> GEngine;