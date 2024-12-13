#pragma once
#include "CoreMinimal.h"

class ENGINE_API UEngine : public UObject
{
public:
    UEngine();
    DEFINE_DEFAULT_CONSTRUCTOR_CALL(UEngine)
    static UClass* StaticClass() { return UEngineClass; }
    static inline UClass* UEngineClass =
        TGetPrivateStaticClassBody<UEngine>(TEXT("UEngine"),
            InternalConstructor<UEngine>, &UObject::StaticClass);
};