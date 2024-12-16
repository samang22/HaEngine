#include "UObject/UObjectBase.h"


#pragma warning(disable:26495)
UObjectBase::UObjectBase()
{
}
#pragma warning(default:26495)

UObjectBase::UObjectBase(EObjectFlags InObjectFlags, UClass* InClass, UObject* InOuter)
	: ObjectFlags(InObjectFlags), ClassPrivate(InClass), OuterPrivate(InOuter)
{
}

UObjectBase::~UObjectBase()
{
}
