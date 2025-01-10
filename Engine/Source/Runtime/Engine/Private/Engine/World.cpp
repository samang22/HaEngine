#include "Engine/World.h"
#include "Engine/Level.h"

void UWorld::InitalizeNewWorld()
{
	PersistentLevel = NewObject<ULevel>(this, ULevel::StaticClass(), TEXT("PersistentLevel"));
}
