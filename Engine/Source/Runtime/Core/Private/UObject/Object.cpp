#include "UObject/Object.h"
#include "UObject/Class.h"

UClass* UObjectRegisterEngineClass = TGetPrivateStaticClassBody<UObject>(
	TEXT("UObject"), InternalConstructor<UObject>, nullptr, nullptr);

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

UWorld* UObject::GetWorld() const
{
	if (UObject* Outer = GetOuter())
	{
		return Outer->GetWorld();
	}

	return nullptr;
}
