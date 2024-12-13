#pragma once
#include "UObjectBase.h"
class UClass;
class CORE_API UObject : public UObjectBase
{
public:
    static UClass* StaticClass() { return nullptr; }
};
