#include "UObject/Object.h"
#include "UObject/Class.h"
#include "UObject/UObjectGlobals.h"
#include "Logging/Logger.h"

UClass* UObjectRegisterEngineClass = TGetPrivateStaticClassBody<UObject>(
	TEXT("UObject"), InternalConstructor<UObject>, nullptr, nullptr);

void UObject::Serialize(FArchive& Ar)
{
    struct FPropertyArchiveData
    {
        map<string, meta::data> Data;
        map<string, FProperty> Prop;
    };
    FPropertyArchiveData PropertyArchiveData;
    type Type = resolve(Hash(GetClass()->ClassName.data()));
    Type.data([&](meta::data Data)
        {
            Data.prop([&](meta::prop p)
                {
                    FProperty Prop = p.value().cast<FProperty>();
                    PropertyArchiveData.Data.emplace(Prop.Name, Data);
                    PropertyArchiveData.Prop.emplace(Prop.Name, Prop);
                }
            );
        }
    );

    if (Ar.IsSaving())
    {
        FString ObjectName = GetName();
        Ar << ObjectName;

        uint64 PropSize = PropertyArchiveData.Prop.size();
        Ar << PropSize;
        for (auto& It : PropertyArchiveData.Prop)
        {
            Ar << It.second.Name;

            switch (It.second.PropertyType)
            {
            case EPropertyType::T_INT:
            {
                int* Value = (int*)PropertyArchiveData.Data[It.second.Name].get(handle(Type.GetNode(), this)).data();
                Ar << *Value;
                break;
            }
            }
        }
    }
    else
    {
        uint64 PropSize = 0;
        Ar << PropSize;
        for (uint64 i = 0; i < PropSize; ++i)
        {
            string PropName;
            Ar << PropName;

            if (PropertyArchiveData.Prop.contains(PropName))
            {
                FProperty& Prop = PropertyArchiveData.Prop[PropName];
                switch (Prop.PropertyType)
                {
                case EPropertyType::T_INT:
                {
                    int* Value = (int*)PropertyArchiveData.Data[Prop.Name].get(handle(Type.GetNode(), this)).data();
                    Ar << *Value;
                }
                }
            }
        }
    }
}


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
