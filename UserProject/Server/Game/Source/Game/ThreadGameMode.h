#pragma once
#include "EngineMinimal.h"
#include "ThreadGameMode.generated.h"

UCLASS()
class GAMEPROJECT_API AThreadGameMode : public AGameModeBase
{
    GENERATED_BODY()
public:
    AThreadGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSceonds) override;

protected:
    std::jthread Thread;
    std::jthread Thread2;
};