#include "Engine/LocalPlayer.h"

void ULocalPlayer::PlayerAdded(UGameViewportClient* InViewportClient)
{
    ViewportClient = InViewportClient;
}