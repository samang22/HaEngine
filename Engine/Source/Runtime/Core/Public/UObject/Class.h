#pragma once

#include "Object.h"
#include "UObject/UObjectGlobals.h"

class FObjectInitializer;

class CORE_API UClass : public UObject
{
private:
	UClass* SuperClass;

public:
	FString ClassName;
	const type_info& ClassTypeInfo;
	const uint64 ClassSize;


public:
	// 오버라이드할 클래스 정보가 있을 경우, 기존 UE에서 쓰던 것처럼
	// FObjectInitializer를 활용하여 덮어쓸것이다.
	using ClassConstructorType = function<void(const FObjectInitializer&)>;
	ClassConstructorType ClassConstructor;

	// 이 Class의 부모 Class의 타입을 얻어오는 함수
	using StaticClassFunctionType = function<UClass* ()>;

public:
	UClass() = delete;
	// ClassSize를 받는 것은, 메모리 풀을 사용해 객체를 생성하여 Cache 히트 확률을 올리기 위해서이다.
	UClass(FString InClassName, const type_info& InClassTypeInfo, 
		const uint64 InClassSize, ClassConstructorType InClassConstructorType,
		StaticClassFunctionType InSuperClassFunction);

	virtual ~UClass() {}
};

CORE_API UClass* GetPrivateStaticClassBody(
	FString InClassName,
	UClass::ClassConstructorType InClassConstructor,
	UClass::StaticClassFunctionType InSuperClassFn,
	//function<void()> InClassReflection,
	const type_info& InClassTypeInfo,
	const uint64 InClassSize
);

template<class T>
UClass* TGetPrivateStaticClassBody(
	FString InClassName,
	UClass::ClassConstructorType InClassConstructor,
	UClass::StaticClassFunctionType InSuperClassFn)
{
	return GetPrivateStaticClassBody(InClassName, InClassConstructor, InSuperClassFn,
		typeid(T), sizeof(T));
}

/**
 * 클래스의 기본 생성자를 호출하기 위한 헬퍼 템플릿입니다.
 */
template<class T>
void InternalConstructor(const FObjectInitializer& X)
{
	FObjectInitializer* ObjectInitializer = const_cast<FObjectInitializer*>(&X);
	ObjectInitializer->SharedObj = allocate_shared<T>(FAllocator<T>(ObjectInitializer));

}