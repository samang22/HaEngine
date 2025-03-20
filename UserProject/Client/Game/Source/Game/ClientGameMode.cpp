#include "ClientGameMode.h"
#include "IpNetDriver.h"

AClientGameMode::AClientGameMode()
{
}

void AClientGameMode::BeginPlay()
{
    Super::BeginPlay();

    NetDriver = NewObject<UIpNetDriver>(this, UIpNetDriver::StaticClass(), TEXT("NetDriver0"));
    FURL URL;
    URL.Host = TEXT("127.0.0.1");
    URL.Port = 5555;
    FString Error;
    if (!NetDriver->InitConnect(this, URL, Error))
    {
        E_LOG(Error, TEXT("Client connect failed: {}"), Error);
        RequestEngineExit(TEXT("Client connect failed"));
        return;
    }
}

void AClientGameMode::Tick(float DeltaSceonds)
{
    Super::Tick(DeltaSceonds);
    NetDriver->Tick(DeltaSceonds);
}