#pragma once
#include "CoreMinimal.h"

class UEngine : public UObject
{
public:
    static UClass* StaticClass() { return UEngineClass; }
    static inline UClass* UEngineClass =
        TGetPrivateStaticClassBody<UEngine>(TEXT("UEngine"),
            InternalConstructor<UEngine>, &UObject::StaticClass);
};