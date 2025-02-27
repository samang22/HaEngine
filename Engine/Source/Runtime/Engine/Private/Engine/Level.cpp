#include "Engine/Level.h"
#include "GameFramework/Actor.h"

ULevel::ULevel()
{
	if (HasAnyFlags(RF_ClassDefaultObject)) { return; }
	OwningWorld = Cast<UWorld>(GetOuter());
}

void ULevel::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    uint64 ActorSize = Ar.IsLoading() ? 0 : Actors.size();
    Ar << ActorSize;

    if (Ar.IsSaving())
    {
        if (Ar.IsPersistent())
        {
            for (TEnginePtr<AActor> Actor : Actors)
            {
                Actor->Serialize(Ar);
            }
        }
        else
        {
            for (TObjectPtr<AActor> Actor : Actors)
            {
                Ar << Actor;
            }
        }
    }
    else
    {
        Actors.reserve(ActorSize);
        for (uint64 i = 0; i < ActorSize; ++i)
        {
            TObjectPtr<AActor> Actor;
            Ar << Actor;
            Actors.push_back(Actor);
        }
    }
}
