#pragma once
#include "EngineMinimal.h"
#include "NetworkDelegates.h"
#include "ServerGameMode.generated.h"

class UIpNetDriver;

UCLASS()
class GAMEPROJECT_API AServerGameMode : public AGameModeBase, public FNetworkNotify
{
	GENERATED_BODY()
public:
	AServerGameMode();
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaSceonds) override;

protected:
	TObjectPtr<UIpNetDriver> NetDriver;
};