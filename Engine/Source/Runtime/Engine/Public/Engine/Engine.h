#pragma once
#include "CoreMinimal.h"
#include "Engine/World.h"
#include "Engine.generated.h"


UCLASS()
class ENGINE_API UEngine : public UObject
{
    GENERATED_BODY()
public:
    UEngine();
    void Init(HWND hViewportHandle);

protected:
    HWND MainViewportHandle = NULL;
    shared_ptr<UWorld> World;
    shared_ptr<UWorld> EditorWorld;
};

extern ENGINE_API UWorld* GWorld;