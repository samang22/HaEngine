#include "UObject/Class.h"
#include "UObject/UObjectArray.h"

map<FString, UClass*> ClassMap;

UClass::UClass(FString InClassName, const type_info& InClassTypeInfo, const uint64 InClassSize, 
	ClassConstructorType InClassConstructorType, StaticClassFunctionType InSuperClassFunction)
	: SuperClass(nullptr)
	, ClassName(InClassName)
	, ClassTypeInfo(InClassTypeInfo)
	, ClassSize(InClassSize)
	, ClassConstructor(InClassConstructorType)
{
	if (InSuperClassFunction)
	{
		SuperClass = InSuperClassFunction();
	}
}
extern FUObjectArray GUObjectArray;

UClass* GetPrivateStaticClassBody(FString InClassName,
    UClass::ClassConstructorType InClassConstructor,
    UClass::StaticClassFunctionType InSuperClassFn,
    const type_info& InClassTypeInfo, const uint64 InClassSize)
{
	GUObjectArray.Create(typeid(UClass), sizeof(UClass));
	GUObjectArray.Create(InClassTypeInfo, InClassSize);

	UClass* ReturnClass = (UClass*)GUObjectArray.Malloc(typeid(UClass));
	ReturnClass = ::new (ReturnClass)
		UClass
		(
			InClassName,
			InClassTypeInfo,
			InClassSize,
			InClassConstructor,
			InSuperClassFn
		);

	ClassMap.emplace(InClassName, ReturnClass);

	return ReturnClass;
 }