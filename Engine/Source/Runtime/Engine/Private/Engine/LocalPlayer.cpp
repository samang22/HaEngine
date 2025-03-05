#include "Engine/LocalPlayer.h"
#include "Engine/World.h"
#include "GameFramework/PlayerController.h"

void ULocalPlayer::PlayerAdded(UGameViewportClient* InViewportClient)
{
    ViewportClient = InViewportClient;
}

void ULocalPlayer::SpawnPlayActor(UWorld* InWorld)
{
    PlayerController = InWorld->SpawnPlayActor(this/*, ROLE_SimulatedProxy, PlayerURL, UniqueId, OutError, GEngine->GetGamePlayers(InWorld).Find(this)*/);
}
