#pragma once
#include "CoreMinimal.h"
#include "GameMapSettings.generated.h"

UCLASS()
class UGameMapSettings : public UObject
{
	GENERATED_BODY()

public:
	UGameMapSettings();

	UClass* GameInstanceClass = nullptr;
};