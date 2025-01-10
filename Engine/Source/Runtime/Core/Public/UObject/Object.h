#pragma once
#include "UObjectBase.h"
class UClass;
class FObjectInitializer;
class CORE_API UObject : public UObjectBase
{
public:
	static void __DefaultConstructor(const FObjectInitializer& X);
	static UClass* StaticClass();

	bool HasAnyFlags(const EObjectFlags FlagToCheck) const
	{
		return (GetFlags() & FlagToCheck);
	}

private:
	template<typename> friend class FAllocator;

};

template<class T1, class T2>
static shared_ptr<T1> Cast(T2* InObj) { return std::dynamic_pointer_cast<T1>(InObj->shared_from_this()); }
template<class T1, class T2>
static shared_ptr<T1> Cast(shared_ptr<T2> InObj) { return std::dynamic_pointer_cast<T1>(InObj); }
//template<class T1, class T2>
//static EnginePtr<T1> Cast(EnginePtr<T2> InObj) { return std::dynamic_pointer_cast<T1>(InObj.lock()); }