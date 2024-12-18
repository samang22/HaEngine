#include "UObject/UObjectBase.h"


#pragma warning(disable:26495)
UObjectBase::UObjectBase()
	: NamePrivate(NoInit)
{
}
#pragma warning(default:26495)

UObjectBase::UObjectBase(EObjectFlags InObjectFlags, UClass* InClass, UObject* InOuter, FName InName)
	: ObjectFlags(InObjectFlags), ClassPrivate(InClass), OuterPrivate(InOuter), NamePrivate(InName)
{
}

UObjectBase::~UObjectBase()
{
}
