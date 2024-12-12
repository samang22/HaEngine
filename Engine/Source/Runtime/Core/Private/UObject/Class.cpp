#include "UObject/Class.h"

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