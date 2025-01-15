#include "UObject/Object.h"
#include "UObject/Class.h"

UClass* UObjectRegisterEngineClass = TGetPrivateStaticClassBody<UObject>(
	TEXT("UObject"), InternalConstructor<UObject>, nullptr);;

void UObject::PostInitProperties()
{
}
void UObject::__DefaultConstructor(const FObjectInitializer& X)
{
	new((EInternal*)X.GetObj())UObject;
}

UClass* UObject::StaticClass()
{
	return UObjectRegisterEngineClass;
}