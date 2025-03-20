#pragma once
#include "EngineMinimal.h"
#include "NetworkDelegates.h"
#include "ClientGameMode.generated.h"

class UIpNetDriver;

UCLASS()
class GAMEPROJECT_API AClientGameMode : public AGameModeBase, public FNetworkNotify
{
    GENERATED_BODY()
public:
    AClientGameMode();
    virtual void BeginPlay() override;
    virtual void Tick(float DeltaSceonds) override;

protected:
    TObjectPtr<UIpNetDriver> NetDriver;
};