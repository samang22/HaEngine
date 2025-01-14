#pragma once
#include "UObjectBase.h"
class UClass;
class FObjectInitializer;
class CORE_API UObject : public UObjectBase
{
public:
	static void __DefaultConstructor(const FObjectInitializer& X);
	static UClass* StaticClass();

	template<class T>
	shared_ptr<T> As()
	{
		if (!this)
		{
			_ASSERT(false);
			return nullptr;
		}
		return CastChecked<T>(shared_from_this());
	}

	/** 이 객체가 지정된 타입인지 여부를 반환합니다. */
	template <typename OtherClassType>
	FORCEINLINE bool IsA(OtherClassType SomeBase) const
	{
		// UObjectBaseUtility와 UClass 사이에 순환 종속성이 있습니다.
		// 이를 해결하기 위해 템플릿을 사용하여 아직 보지 않은 것을 인라인화할 수 있도록 합니다.
		// 이는 함수가 호출될 때까지 컴파일을 지연시키기 때문입니다.

		// 'static_assert'로 이 객체가 실제로 UClass 포인터인지 또는 변환 가능한지 확인합니다.
		const UClass* SomeBaseClass = SomeBase;
		//(void)SomeBaseClass;
		_ASSERT(SomeBaseClass/*, TEXT("IsA(NULL)은 의미 있는 결과를 낼 수 없습니다")*/);

		const UClass* ThisClass = GetClass();

		// 컴파일러가 불필요한 nullptr 체크 분기를 하지 않도록 합니다.
		//UE_ASSUME(SomeBaseClass);
		//UE_ASSUME(ThisClass);

		return IsChildOfWorkaround(ThisClass, SomeBaseClass);
	}

	bool HasAnyFlags(const EObjectFlags FlagToCheck) const
	{
		return (GetFlags() & FlagToCheck);
	}
private:
	template <typename ClassType>
	static FORCEINLINE bool IsChildOfWorkaround(const ClassType* ObjClass, const ClassType* TestCls)
	{
		return ObjClass->IsChildOf(TestCls);
	}

private:
	template<typename> friend class FAllocator;

};

template<class T1, class T2>
static shared_ptr<T1> Cast(T2* InObj) { return std::dynamic_pointer_cast<T1>(InObj->shared_from_this()); }
template<class T1, class T2>
static shared_ptr<T1> Cast(shared_ptr<T2> InObj) { return std::dynamic_pointer_cast<T1>(InObj); }
template<class T1, class T2>
static EnginePtr<T1> Cast(EnginePtr<T2> InObj) { return std::dynamic_pointer_cast<T1>(InObj.lock()); }

template<class T1, class T2>
static shared_ptr<T1> CastChecked(T2* InObj)
{
	shared_ptr<T1> CastResult = std::dynamic_pointer_cast<T1>(InObj->shared_from_this());
	_ASSERT(CastResult.get());
	return CastResult;
}
template<class T1, class T2>
static T1* CastCheckedRaw(T2* InObj)
{
	T1* CastResult = dynamic_cast<T1*>(InObj);
	_ASSERT(CastResult);
	return CastResult;
}
template<class T1, class T2>
static shared_ptr<T1> CastChecked(shared_ptr<T2> InObj)
{
	shared_ptr<T1> CastResult = std::dynamic_pointer_cast<T1>(InObj);
	_ASSERT(CastResult.get());
	return CastResult;
}
template<class T1, class T2>
static EnginePtr<T1> CastChecked(EnginePtr<T2> InObj)
{
	EnginePtr<T1> CastResult = std::dynamic_pointer_cast<T1>(InObj.lock());
	CastResult.Get();
	return CastResult;
}