#pragma once
#pragma warning(disable: 4251)
#pragma warning(disable: 4530) // exception warning
#include <string>
#include <format>
#include <memory>
#include <typeinfo>
#include <functional>
#include <codecvt>
#include <filesystem>
#include <chrono>
#include <span>
#include <cmath>

#define NOMINMAX
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

// Containers --------------
#include <vector>
#include <map>
#include <unordered_map>
#include <queue>
// Containers End --------------

#include "UObject/UObjectMacros.h"
#include "HAL/Platform.h"
#include "UObject/NameTypes.h"

using namespace std;
using FString = std::wstring;
using FStringView = std::wstring_view;

static inline const WIDECHAR* NAME_NONE = TEXT("");