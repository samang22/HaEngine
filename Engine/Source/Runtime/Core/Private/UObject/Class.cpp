#include "UObject/Class.h"
#include "UObject/UObjectArray.h"

CORE_API map<FString, UClass*> ClassMap;

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
UObject* UClass::CreateDefaultObject()
{
	_ASSERT(!ClassDefaultObject);

	FStaticConstructObjectParameters StaticConstructObjectParameters(this);
	StaticConstructObjectParameters.SetFlags = EObjectFlags::RF_ClassDefaultObject;
	StaticConstructObjectParameters.Name = FName(ClassName);
	ClassConstructor(FObjectInitializer(ClassDefaultObject, StaticConstructObjectParameters));

	return ClassDefaultObject.get();
}
void UClass::InternalCreateDefaultObjectWrapper() const
{
	// 객체의 상수성을 일시적으로 제한하기 위해 
	// 내부 전용 함수를 만들고, const_cast를 사용
	const_cast<UClass*>(this)->CreateDefaultObject();
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