#pragma once
#include "CoreMinimal.h"
#include "GameMapSettings.generated.h"

UCLASS()
class ENGINE_API UGameMapSettings : public UObject {
	GENERATED_BODY()

public:
	UGameMapSettings();

	UClass* GameInstanceClass = nullptr;
	UClass* GameModeClass = nullptr;
};