#pragma once
#include "UObject/Object.h"
//#include "Logging/Logger.h"

template<class ... Args>
class FDelegate
{
public:
	template<class TType>
	void AddRow(TType* NewInstance, void(TType::* NewFunction)(Args ...))
	{
		FFunctionType Function;
		constexpr uint32 Argc = sizeof ...(Args);
		if constexpr (Argc == 0) { Function = bind(NewFunction, NewInstance); }
		else if constexpr (Argc == 1) { Function = bind(NewFunction, NewInstance, std::placeholders::_1); }
		else if constexpr (Argc == 2) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2); }
		else if constexpr (Argc == 3) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3); }
		else if constexpr (Argc == 4) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4); }
		else if constexpr (Argc == 5) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5); }
		else if constexpr (Argc == 6) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6); }
		else if constexpr (Argc == 7) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7); }
		else if constexpr (Argc == 8) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8); }
		else if constexpr (Argc == 9) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9); }
		else if constexpr (Argc == 10) { Function = bind(NewFunction, NewInstance, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4, std::placeholders::_5, std::placeholders::_6, std::placeholders::_7, std::placeholders::_8, std::placeholders::_9, std::placeholders::_10); }

		Functions.emplace_back(NewInstance, Function);
	}

	void Broadcast(Args... InArgs)
	{
		for (auto It : Functions)
		{
			It.second(forward<Args>(InArgs)...);
		}
	}

	void Clear()
	{
		Functions.clear();
	}

private:
	using FFunctionType = function<void(Args ...)>;
	TArray<pair<void*, FFunctionType>> Functions;
};