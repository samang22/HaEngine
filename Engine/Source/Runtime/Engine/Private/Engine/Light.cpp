#include "Engine/Light.h"

ALight::ALight(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

void ALight::OnPropertyChanged(FProperty& Property)
{
    Super::OnPropertyChanged(Property);
}