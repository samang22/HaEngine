#pragma once
#include "CoreTypes.h"
#include "UObjectMacros.h"
#include "NameTypes.h"

class UClass;
class UObject;
class CORE_API UObjectBase
{
public:
	UObjectBase();
	UObjectBase(EObjectFlags InObjectFlags, UClass* InClass, UObject* InOuter, FName InName);
	virtual ~UObjectBase();

	EObjectFlags GetFlags() const { return ObjectFlags; }
	UObject* GetOuter() const { return OuterPrivate; }
	UClass* GetClass() const { return ClassPrivate; }
	const FName& GetName() const { return NamePrivate; }

private:
	EObjectFlags ObjectFlags;
	UClass* ClassPrivate;
	UObject* OuterPrivate;

	/** Name of this object */
	FName NamePrivate;
};