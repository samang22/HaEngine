#pragma once
#include "CoreTypes.h"
//#include "EngineWeakPtr.h"
//
//bool CORE_API IsEngineExitRequested();
//
//void CORE_API RequestEngineExit(const FString ReasonString);
//
//class UClass;
//class UObject;
//struct CORE_API FStaticConstructObjectParameters
//{
//	UClass* Class = nullptr;
//	UObject* Outer = nullptr;
//	FString Name;
//	EObjectFlags SetFlags = RF_NoFlags;
//
//	/**
//	 * 지정된 경우, 이 객체의 속성 값이 새 객체로 복사되며, 새 객체의 ObjectArchetype 값이 이 객체로 설정됩니다.
//	 * 만약 nullptr인 경우, 클래스 기본 객체(class default object)가 대신 사용됩니다.
//	 */
//	UObject* Template = nullptr;
//
//	FStaticConstructObjectParameters(UClass* InClass);
//};
//
//struct CORE_API FObjectInitializer
//{
//	// FObjectInitializer를 사용해서 신규로 만들어진 UObject 객체
//	shared_ptr<UObject>& SharedObj;
//
//	UClass* Class = nullptr;
//	/** Object this object resides in. */
//	UObject* OuterPrivate = nullptr;
//	EObjectFlags ObjectFlags = EObjectFlags::RF_NoFlags;
//	FString Name;
//
//	// FAllocator 내부에서 MamoryPool로 부터 얻어온 주소(shared_ptr 크기를 고려해서 계산된 주소)
//	UObject* Obj = nullptr;
//	/** 속성을 복사할 객체 **/
//	UObject* ObjectArchetype = nullptr;
//	/**  If true, initialize the properties **/
//	bool bShouldInitializePropsFromArchetype = false;
//
//	UObject* GetObj() const { return Obj; }
//
//	FObjectInitializer(shared_ptr<UObject>& InObj, const FStaticConstructObjectParameters& StaticConstructParams);
//	~FObjectInitializer();
//
//	/**
//	 * 객체 속성을 이진 방식으로 0 또는 기본값으로 초기화합니다.
//	 *
//	 * @param   Obj                 데이터를 초기화할 객체
//	 * @param   DefaultsClass       데이터 초기화에 사용할 클래스
//	 * @param   DefaultData         초기화에 사용할 원본 데이터를 포함하는 버퍼
//	 * @param   bCopyTransientsFromClassDefaults    true인 경우, 클래스 기본값에서 일시적인 데이터를 복사하고, 그렇지 않으면 DefaultData에서 일시적인 데이터를 복사합니다.
//	 */
//	static void InitProperties(UObject* Obj, UClass* DefaultsClass, UObject* DefaultData, bool bCopyTransientsFromClassDefaults);
//
//	/**
//	 * Finalizes a constructed UObject by initializing properties,
//	 * instancing/initializing sub-objects, etc.
//	 */
//	void PostConstructInit();
//};
//
//CORE_API shared_ptr<UObject> StaticConstructObject_Internal(FStaticConstructObjectParameters& Params);
//
//template<typename T>
//shared_ptr<T> NewObject(UObject* Outer, UClass* Class = nullptr, FString Name = NAME_NONE, EObjectFlags Flags = RF_NoFlags)
//{
//	if (!Class)
//	{
//		Class = T::StaticClass();
//	}
//
//	FStaticConstructObjectParameters Params(Class);
//	Params.Outer = Outer;
//	Params.Name = Name;
//	Params.SetFlags = Flags;
//
//	return Cast<T>(StaticConstructObject_Internal(Params));
//}

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
