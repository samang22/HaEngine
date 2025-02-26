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

    /**
     * 최대 틱 속도를 고려하여 FApp::CurrentTime / FApp::DeltaTime을 업데이트합니다.
    */
    void UpdateTimeAndHandleMaxTickRate();

public:
    string Save();
    void Load(const string& InLoadString);

public:
    void WndProc(UINT Message, WPARAM wParam, LPARAM lParam, LRESULT* pResult);

protected:
    /** 틱 레이트 제한기를 가져옵니다. */
    double GetMaxTickRate(double DeltaTime);
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