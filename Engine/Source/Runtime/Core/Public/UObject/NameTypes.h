#pragma once
#include "UObject/UObjectMacros.h"
#include "HAL/Platform.h"
#include <string>
using FStringView = std::wstring_view;
using FString = std::wstring;

/**
 * Public name, available to the world.  Names are stored as a combination of
 * an index into a table of unique strings and an instance number.
 * Names are case-insensitive, but case-preserving (when WITH_CASE_PRESERVING_NAME is 1)
 */
class CORE_API FName
{
public:
	FName() {}
	FName(FStringView InString);
	FName(const WIDECHAR* InString);

	/**
	 * Converts an FName to a readable format
	 *
	 * @return String representation of the name
	 */
	FString ToString() const;

	/**
	 * Converts an FName to a readable format, in place
	 *
	 * @param Out String to fill with the string representation of the name
	 */
	void ToString(FString& Out) const;

	bool operator<(const FName& Other) const { return HashCode < Other.HashCode; }

private:
	uint64 HashCode = 0;
};

const static inline FName NAME_None;