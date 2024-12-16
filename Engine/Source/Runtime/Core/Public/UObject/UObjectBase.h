#pragma once
#include "CoreTypes.h"
#include "UObjectMacros.h"

class UClass;
class UObject;
class CORE_API UObjectBase
{
public:
	UObjectBase();
	UObjectBase(EObjectFlags InObjectFlags, UClass* InClass, UObject* InOuter);
	virtual ~UObjectBase();

	EObjectFlags GetFlags() const { return ObjectFlags; }
	UObject* GetOuter() const { return OuterPrivate; }
	UClass* GetClass() const { return ClassPrivate; }

private:
	EObjectFlags ObjectFlags;
	UClass* ClassPrivate;
	UObject* OuterPrivate;
};