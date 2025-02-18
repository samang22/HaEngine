#include "UObject/Object.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "Logging/Logger.h"

UClass* UObjectRegisterEngineClass = TGetPrivateStaticClassBody<UObject>(
	TEXT("UObject"), InternalConstructor<UObject>, nullptr, nullptr);

UObject* UObject::CreateDefaultSubobject(FName SubobjectFName, UClass* ReturnType, UClass* ClassToCreateByDefault, bool bIsRequired, bool bIsTransient)
{
    if (FObjectInitializer::InitializerStack.empty())
    {
        E_LOG(Fatal, TEXT("FObjectInitializer::InitializerStack is empty."));
        return nullptr;
    }
    FObjectInitializer* CurrentInitializer = FObjectInitializer::InitializerStack[FObjectInitializer::InitializerStack.size() - 1];
    if (!CurrentInitializer)
    {
        E_LOG(Fatal, TEXT("No object initializer found during construction."));
        return nullptr;
    }
    if (CurrentInitializer->Obj != this)
    {
        E_LOG(Fatal, TEXT("Using incorrect object initializer."));
        return nullptr;
    }

    return CurrentInitializer->CreateDefaultSubobject(this, SubobjectFName, ReturnType, ClassToCreateByDefault, bIsRequired, bIsTransient);
}

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
