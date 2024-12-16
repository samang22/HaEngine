#pragma once
#include "UObjectBase.h"
class UClass;
class CORE_API UObject : public UObjectBase
{
public:
    static UClass* StaticClass() { return nullptr; }

private:
	template<typename> friend class FAllocator;
	/** Name of this object */
	FString NamePrivate;
};

template<class T1, class T2>
static shared_ptr<T1> Cast(T2* InObj) { return std::dynamic_pointer_cast<T1>(InObj->shared_from_this()); }
template<class T1, class T2>
static shared_ptr<T1> Cast(shared_ptr<T2> InObj) { return std::dynamic_pointer_cast<T1>(InObj); }
//template<class T1, class T2>
//static engine_weak_ptr<T1> Cast(engine_weak_ptr<T2> InObj) { return std::dynamic_pointer_cast<T1>(InObj.lock()); }