#pragma once
#include "CoreTypes.h"
#include "UObjectMacros.h"
#include "NameTypes.h"
#include "EnginePtr.h"

class UClass;
class UObject;
class CORE_API UObjectBase : public std::enable_shared_from_this<UObjectBase>
{
public:
	UObjectBase();
	UObjectBase(EObjectFlags InObjectFlags, UClass* InClass, UObject* InOuter, FName InName);
	virtual ~UObjectBase();

	EObjectFlags GetFlags() const { return ObjectFlags; }
	UObject* GetOuter() const { return OuterPrivate; }
	UClass* GetClass() const { return ClassPrivate; }

	/**
	* Returns the name of this object (with no path information)
	*
	* @return Name of the object.
	*/
	FORCEINLINE FString GetName() const
	{
		return GetFName().ToString();
	}

	/** Returns the logical name of this object */
	FORCEINLINE FName GetFName() const
	{
		return NamePrivate;
	}

private:
	EObjectFlags ObjectFlags;
	UClass* ClassPrivate;
	UObject* OuterPrivate;

	/** Name of this object */
	FName NamePrivate;
};