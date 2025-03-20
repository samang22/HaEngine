#include "ServerGameMode.h"
#include "IpNetDriver.h"

AServerGameMode::AServerGameMode()
{
}


void AServerGameMode::BeginPlay()
{
    Super::BeginPlay();

    NetDriver = NewObject<UIpNetDriver>(this, UIpNetDriver::StaticClass(), TEXT("NetDriver0"));
    FURL URL;
    FString Error;
    if (!NetDriver->InitListen(this, URL, false, Error))
    {
        E_LOG(Error, TEXT("Server listen failed: {}"), Error);
        RequestEngineExit(TEXT("Server listen failed"));
        return;
    }
}

void AServerGameMode::Tick(float DeltaSceonds)
{
    Super::Tick(DeltaSceonds);
    NetDriver->Tick(DeltaSceonds);
}