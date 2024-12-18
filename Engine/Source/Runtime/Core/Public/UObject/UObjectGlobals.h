#pragma once
#include "CoreTypes.h"
//#include "EngineWeakPtr.h"
//
//bool CORE_API IsEngineExitRequested();
//
//void CORE_API RequestEngineExit(const FString ReasonString);
//
class UClass;
class UObject;
/**
 * 이 구조체는 StaticConstructObject_Internal() 메서드에 매개변수 값을 전달하는 데 사용됩니다. 생성자 매개변수만 유효하면 되며,
 * 다른 모든 멤버는 선택 사항입니다.
 */
struct CORE_API FStaticConstructObjectParameters
{
	UClass* Class = nullptr;
	UObject* Outer = nullptr;
	FName Name;
	EObjectFlags SetFlags = RF_NoFlags;

	/**
	 * 지정된 경우, 이 객체의 속성 값이 새 객체로 복사되며, 새 객체의 ObjectArchetype 값이 이 객체로 설정됩니다.
	 * 만약 nullptr인 경우, 클래스 기본 객체(class default object)가 대신 사용됩니다.
	 */
	UObject* Template = nullptr;

	FStaticConstructObjectParameters(UClass* InClass);
};

class CORE_API FObjectInitializer
{
public:
	// FObjectInitializer를 사용해서 신규로 만들어진 UObject 객체
	shared_ptr<UObject>& SharedObj;

	UClass* Class = nullptr;
	/** Object this object resides in. */
	UObject* OuterPrivate = nullptr;
	EObjectFlags ObjectFlags = EObjectFlags::RF_NoFlags;
	FName Name;

	// FAllocator 내부에서 MamoryPool로 부터 얻어온 주소(shared_ptr 크기를 고려해서 계산된 주소)
	UObject* Obj = nullptr;
	/** 속성을 복사할 객체 **/
	UObject* ObjectArchetype = nullptr;
	/**  If true, initialize the properties **/
	bool bShouldInitializePropsFromArchetype = false;

	UObject* GetObj() const { return Obj; }

	FObjectInitializer(shared_ptr<UObject>& InObj, const FStaticConstructObjectParameters& StaticConstructParams);
	~FObjectInitializer();

	/**
	 * 객체 속성을 이진 방식으로 0 또는 기본값으로 초기화합니다.
	 *
	 * @param   Obj                 데이터를 초기화할 객체
	 * @param   DefaultsClass       데이터 초기화에 사용할 클래스
	 * @param   DefaultData         초기화에 사용할 원본 데이터를 포함하는 버퍼
	 * @param   bCopyTransientsFromClassDefaults    true인 경우, 클래스 기본값에서 일시적인 데이터를 복사하고, 그렇지 않으면 DefaultData에서 일시적인 데이터를 복사합니다.
	 */
	 //static void InitProperties(UObject* Obj, UClass* DefaultsClass, UObject* DefaultData, bool bCopyTransientsFromClassDefaults);

	 /**
	  * Finalizes a constructed UObject by initializing properties,
	  * instancing/initializing sub-objects, etc.
	  */
	  //void PostConstructInit();
};

CORE_API shared_ptr<UObject> StaticConstructObject_Internal(FStaticConstructObjectParameters& Params);

template<typename T>
shared_ptr<T> NewObject(UObject* Outer, UClass* Class = nullptr, FName Name = NAME_NONE, EObjectFlags Flags = RF_NoFlags)
{
	if (!Class)
	{
		Class = T::StaticClass();
	}

	FStaticConstructObjectParameters Params(Class);
	Params.Outer = Outer;
	Params.Name = Name;
	Params.SetFlags = Flags;

	return Cast<T>(StaticConstructObject_Internal(Params));
}

// wstring -> UTF8
CORE_API string to_string(const FString& InString);
// wstring -> UTF8
CORE_API string to_string(FStringView InString);
// UTF8 -> wstring
CORE_API FString to_wstring(string_view InString);
#define ANSI_TO_TCHAR to_wstring

CORE_API uint64 operator""_hash(const ANSICHAR * NewString, size_t);
CORE_API uint64 operator""_hash(const WIDECHAR * NewString, size_t);
CORE_API uint64 Hash(const ANSICHAR* NewString);
CORE_API uint64 Hash(const WIDECHAR* NewString);
//
///**
// * Get default object of a class.
// * @see UClass::GetDefaultObject()
// */
//template< class T >
//inline const T* GetDefault()
//{
//	return (const T*)T::StaticClass()->GetDefaultObject();
//}
//
//extern CORE_API map<UClass*, vector<engine_weak_ptr<UObject>>> ObjectMap;
//
///** Enum used in StaticDuplicateObject() and related functions to describe why something is being duplicated */
//namespace EDuplicateMode
//{
//	enum Type
//	{
//		/** No specific information about the reason for duplication */
//		Normal,
//		/** Object is being duplicated as part of a world duplication */
//		World,
//		/** Object is being duplicated as part of the process for entering Play In Editor */
//		PIE
//	};
//};
//
//CORE_API shared_ptr<UObject> StaticDuplicateObject(UObject* SourceObject, UObject* DestOuter, const FString DestName, EDuplicateMode::Type DuplicateMode = EDuplicateMode::Normal);


#include "UObjectArray.h"
#include "Object.h"

template <class _Ty>
class FAllocator
{
public:
	static_assert(!is_const_v<_Ty>, "The C++ Standard forbids containers of const elements "
		"because allocator<const T> is ill-formed.");
	static_assert(!is_function_v<_Ty>, "The C++ Standard forbids allocators for function elements "
		"because of [allocator.requirements].");
	static_assert(!is_reference_v<_Ty>, "The C++ Standard forbids allocators for reference elements "
		"because of [allocator.requirements].");

	using _From_primary = FAllocator;

	using value_type = _Ty;
	using size_type = size_t;
	using difference_type = ptrdiff_t;

	using propagate_on_container_move_assignment = true_type;
	using is_always_equal _CXX20_DEPRECATE_IS_ALWAYS_EQUAL = true_type;
	FAllocator(FObjectInitializer* InObjectInitializer) noexcept :
		Data(InObjectInitializer)
	{
	}
	constexpr FAllocator(const FAllocator&) noexcept = default;
	template <class _Other>
	constexpr FAllocator(const FAllocator<_Other>& InOther) noexcept
		: Data(InOther.Data.ObjectInitializer)
	{
	}
	_CONSTEXPR20 ~FAllocator() = default;
	_CONSTEXPR20 FAllocator& operator=(const FAllocator&) = default;

	_NODISCARD_RAW_PTR_ALLOC _CONSTEXPR20 __declspec(allocator) _Ty* allocate(_CRT_GUARDOVERFLOW const size_t /*_Count*/) {
		static_assert(sizeof(value_type) > 0, "value_type must be complete before calling allocate.");
		_Ty* Pointer = (_Ty*)GUObjectArray.Malloc(Data.ObjectInitializer->Class->ClassTypeInfo);
		return Pointer;
	}

	template <class _Objty, class... _Types>
	_CXX17_DEPRECATE_OLD_ALLOCATOR_MEMBERS void construct(_Objty* const _Ptr, _Types&&... _Args)
	{
		Data.ObjectInitializer->Obj = _Ptr;
		new(Data.ObjectInitializer->GetObj())UObjectBase(
			Data.ObjectInitializer->ObjectFlags,
			Data.ObjectInitializer->Class,
			Data.ObjectInitializer->OuterPrivate,
			Data.ObjectInitializer->Name
		);
		_Objty::__DefaultConstructor(*Data.ObjectInitializer);
	}

	template< class U >
	_CONSTEXPR20 void destroy(U* p)
	{
		UClass* Class = p->GetClass();
		Data.DestructorClass = Class;
		_ASSERT(Class);
		p->~U();
	}
	_CONSTEXPR20 void deallocate(_Ty* const _Ptr, const size_t _Count) {
		_STL_ASSERT(_Ptr != nullptr || _Count == 0, "null pointer cannot point to a block of non-zero size");
		_STL_ASSERT(_Count == 1, "error");
		GUObjectArray.Free(Data.DestructorClass->ClassTypeInfo, _Ptr);
	}

public:
	union FData
	{
		FObjectInitializer* ObjectInitializer;
		UClass* DestructorClass;
	};
	FData Data;
};
