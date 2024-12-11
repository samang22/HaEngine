#include "UObject/UObjectGlobals.h"
//#include "UObject/Class.h"
//#include "UObject/UObject.h"
//#include "Logging/Logger.h"
//
//bool GIsRequestingExit = false; /* Indicates that MainLoop() should be exited at the end of the current iteration */
//
//CORE_API map<UClass*, vector<engine_weak_ptr<UObject>>> ObjectMap;
//
//bool IsEngineExitRequested()
//{
//	return GIsRequestingExit;
//}
//
//void CORE_API RequestEngineExit(const FString ReasonString)
//{
//	E_LOG(Trace, TEXT("{}"), ReasonString.c_str());
//	GIsRequestingExit = true;
//}
//
//FStaticConstructObjectParameters::FStaticConstructObjectParameters(UClass* InClass)
//	: Class(InClass)
//{
//	if (!Class)
//	{
//		E_LOG(Error, TEXT("Class is nullptr"))
//	}
//}
//
//FObjectInitializer::FObjectInitializer(shared_ptr<UObject>& InObj, const FStaticConstructObjectParameters& StaticConstructParams)
//	: SharedObj(InObj)
//	, Class(StaticConstructParams.Class)
//	, OuterPrivate(StaticConstructParams.Outer)
//	, ObjectFlags(StaticConstructParams.SetFlags)
//	, Name(StaticConstructParams.Name)
//	, ObjectArchetype(StaticConstructParams.Template)
//{
//	if (ObjectArchetype)
//	{
//		bShouldInitializePropsFromArchetype = true;
//	}
//}
//
//FObjectInitializer::~FObjectInitializer()
//{
//	PostConstructInit();
//}
//#include "Math/SimpleMath.h"
//void FObjectInitializer::InitProperties(UObject* Obj, UClass* DefaultsClass, UObject* DefaultData, bool bCopyTransientsFromClassDefaults)
//{
//	if (DefaultData)
//	{
//		type Type = resolve(Hash(DefaultData->GetClass()->ClassName.data()));
//		Type.data([&](meta::data Data)
//			{
//				Data.prop([&](meta::prop p)
//					{
//						FProperty Prop = p.value().cast<FProperty>();
//						void* Dest = Data.get(handle(Type.GetNode(), Obj)).data();
//						void* Src = Data.get(handle(Type.GetNode(), DefaultsClass)).data();
//						switch (Prop.PropertyType)
//						{
//						case T_FVector:
//						{
//							FVector* _Dest = (FVector*)Dest;
//							FVector* _Src = (FVector*)Src;
//							*_Dest = *_Src;
//							break;
//						}
//						case T_FRotator:
//						{
//							FRotator* _Dest = (FRotator*)Dest;
//							FRotator* _Src = (FRotator*)Src;
//							*_Dest = *_Src;
//							break;
//						}
//						}
//					});
//			});
//	}
//}
//
//void FObjectInitializer::PostConstructInit()
//{
//	if (bShouldInitializePropsFromArchetype)
//	{
//		UClass* BaseClass = /*(bIsCDO && !GIsDuplicatingClassForReinstancing) ? SuperClass :*/ Class;
//		if (BaseClass == NULL)
//		{
//			_ASSERT(Class == UObject::StaticClass());
//			BaseClass = Class;
//		}
//
//		UObject* Defaults = ObjectArchetype ? ObjectArchetype : BaseClass->GetDefaultObject(false); // we don't create the CDO here if it doesn't already exist
//		InitProperties(Obj, BaseClass, Defaults, false/*bCopyTransientsFromClassDefaults*/);
//	}
//
//	// InitSubobjectProperties
//	{
//	}
//
//	SharedObj->PostInitProperties();
//}
//
//CORE_API shared_ptr<UObject> StaticConstructObject_Internal(FStaticConstructObjectParameters& Params)
//{
//	UClass* InClass = Params.Class;
//	FString& InName = Params.Name;
//
//	if (InName == NAME_NONE)
//	{
//		static map<FString, int64> NameCountMap;
//		FString ClassName = InClass->ClassName;
//		int64& NewIndex = NameCountMap[ClassName];
//
//		InName = ClassName + TEXT("_") + to_wstring(NewIndex);
//
//		++NewIndex;
//	}
//
//	shared_ptr<UObject> Result = NULL;
//
//	InClass->ClassConstructor(FObjectInitializer(Result, Params));
//
//	auto& ObjectVector = ObjectMap[InClass];
//	ObjectVector.emplace_back(Result);
//
//	return Result;
//}

CORE_API string to_string(const FString& InString)
{
	// wstring -> UTF8
	return wstring_convert<codecvt_utf8<WIDECHAR>>().to_bytes(InString);
}

CORE_API string to_string(FStringView InString)
{
	// wstring -> UTF8
	return wstring_convert<codecvt_utf8<WIDECHAR>>().to_bytes(InString.data());
}

CORE_API FString to_wstring(string_view InString)
{
	// UTF8 -> wstring
	return wstring_convert<codecvt_utf8<WIDECHAR>>().from_bytes(InString.data());
}

CORE_API uint64 operator""_hash(const ANSICHAR * NewString, size_t)
{
	// 문자열을 Hash function에 넣으면 숫자가 나온다
	// 같은 문자열을 넣으면 같은 숫자가 난온다
	// Hash 충돌: 그러나 가끔 다른 문자열을 넣었을때 동일한 숫자가 나올수도 있다.
	static hash<string_view> HashFunction;
	return HashFunction(NewString);
}

CORE_API uint64 operator""_hash(const WIDECHAR * NewString, size_t)
{
	static hash<wstring_view> HashFunction;
	return HashFunction(NewString);
}

CORE_API uint64 Hash(const ANSICHAR* NewString)
{
	static hash<string_view> HashFunction;
	return HashFunction(NewString);
}

CORE_API uint64 Hash(const WIDECHAR* NewString)
{
	static hash<wstring_view> HashFunction;
	return HashFunction(NewString);
}

//
//#include <boost/archive/text_oarchive.hpp>
//#include <boost/archive/text_iarchive.hpp>
//#include <boost/serialization/nvp.hpp>
//
//shared_ptr<UObject> StaticDuplicateObject(UObject* SourceObject, UObject* DestOuter, const FString DestName, EDuplicateMode::Type DuplicateMode)
//{
//	// Parameters.SourceObject->PreDuplicate(Parameters);
//
//	FStaticConstructObjectParameters Params(SourceObject->GetClass());
//	Params.Outer = DestOuter;
//	Params.Name = DestName;
//	Params.SetFlags = SourceObject->GetFlags();
//	Params.Template = SourceObject;
//	shared_ptr<UObject> DupRootObject = StaticConstructObject_Internal(Params);
//
//	map<UObject*, shared_ptr<UObject>> DuplicatedObjectAnnotation;
//	std::stringstream Buffer;
//	boost::archive::text_oarchive WriteAr = boost::archive::text_oarchive(Buffer);
//	FArchive Writer = FArchive(WriteAr, DuplicatedObjectAnnotation, SourceObject, DupRootObject);
//	
//	vector<shared_ptr<UObject>> SerializedObjects;
//
//	while (Writer.UnserializedObjects.size())
//	{
//		shared_ptr<UObject> Object = Writer.UnserializedObjects.back();
//		SerializedObjects.push_back(Object);
//		Writer.UnserializedObjects.pop_back();
//		Object->Serialize(Writer);
//	};
//
//	boost::archive::text_iarchive ReadAr = boost::archive::text_iarchive(Buffer);
//	FArchive Reader = FArchive(ReadAr, DuplicatedObjectAnnotation);
//	for (int32 ObjectIndex = 0; ObjectIndex < SerializedObjects.size(); ObjectIndex++)
//	{
//		shared_ptr<UObject> SerializedObject = SerializedObjects[ObjectIndex];
//		shared_ptr<UObject> DuplicatedObject = DuplicatedObjectAnnotation[SerializedObject.get()];
//		_ASSERT(DuplicatedObject);
//
//		if (!DuplicatedObject->HasAnyFlags(RF_ClassDefaultObject))
//		{
//			DuplicatedObject->Serialize(Reader);
//		}
//		else
//		{
//			// 고민해보기
//			_ASSERT(false);
//		}
//	}
//
//	for (int32 ObjectIndex = 0; ObjectIndex < SerializedObjects.size(); ObjectIndex++)
//	{
//		shared_ptr<UObject> OrigObject = SerializedObjects[ObjectIndex];
//		shared_ptr<UObject> DuplicatedObject = DuplicatedObjectAnnotation[OrigObject.get()];
//		DuplicatedObject->PostDuplicate(DuplicateMode);
//		if (/*!Parameters.bSkipPostLoad && */!DuplicatedObject->IsTemplate())
//		{
//			DuplicatedObject->PostLoad(); // ConditionalPostLoad();
//		}
//	}
//
//	return DupRootObject;
//}
