#pragma once
#include "CoreTypes.h"

// weak_ptr의 사용성을 개선한 EnginePtr 입니다.
template<class _Ty>
class EnginePtr : public weak_ptr<_Ty>
{
public:
	EnginePtr() noexcept {}
	EnginePtr(const EnginePtr& InOther) noexcept : weak_ptr<_Ty>(InOther) {}
	template <class _Ty2, enable_if_t<_SP_pointer_compatible<_Ty2, _Ty>::value, int> = 0>
	EnginePtr(const weak_ptr<_Ty2>& InOther) noexcept : weak_ptr<_Ty>(InOther) {}

	template <class _Ty2, enable_if_t<_SP_pointer_compatible<_Ty2, _Ty>::value, int> = 0>
	EnginePtr(const shared_ptr<_Ty2>& InOther) noexcept : weak_ptr<_Ty>(InOther) {}

	EnginePtr& operator=(nullptr_t) noexcept {
		this->reset();
		return *this;
	}

	EnginePtr& operator=(const EnginePtr& InOther) noexcept
	{
		EnginePtr(InOther).swap(*this);
		return *this;
	}

	template <class _Ty2, enable_if_t<_SP_pointer_compatible<_Ty2, _Ty>::value, int> = 0>
	EnginePtr& operator=(const EnginePtr<_Ty2>& _Right) noexcept {
		EnginePtr(_Right).swap(*this);
		return *this;
	}

	EnginePtr& operator=(EnginePtr&& _Right) noexcept {
		EnginePtr(_STD move(_Right)).swap(*this);
		return *this;
	}

	template <class _Ty2, enable_if_t<_SP_pointer_compatible<_Ty2, _Ty>::value, int> = 0>
	EnginePtr& operator=(EnginePtr<_Ty2>&& _Right) noexcept {
		EnginePtr(_STD move(_Right)).swap(*this);
		return *this;
	}

	template <class _Ty2, enable_if_t<_SP_pointer_compatible<_Ty2, _Ty>::value, int> = 0>
	EnginePtr& operator=(const shared_ptr<_Ty2>& _Right) noexcept {
		EnginePtr(_Right).swap(*this);
		return *this;
	}

	template <class _Ty2 = _Ty, enable_if_t<!disjunction_v<is_array<_Ty2>, is_void<_Ty2>>, int> = 0>
	_NODISCARD _Ty2& operator*() const noexcept {
		return *Get();
	}

	template <class _Ty2 = _Ty, enable_if_t<!is_array_v<_Ty2>, int> = 0>
	_NODISCARD _Ty2* operator->() const noexcept {
		return Get();
	}
	template <class _Ty2 = _Ty, enable_if_t<!is_array_v<_Ty2>, int> = 0>
	_NODISCARD _Ty2* Get() const noexcept {
#if WITH_DEBUG
		if (!IsValid())
		{
			_ASSERT(false);
		}
#endif
		return this->lock().get();
	}

	explicit operator bool() const noexcept
	{
		return IsValid();
	}

	bool IsValid() const noexcept
	{
		if (!this) { return false; }
		return !this->expired();
	}
};

template<class _Ty1, class _Ty2>
bool operator==(const EnginePtr<_Ty1> _Left, const EnginePtr<_Ty2> _Right)
{
	return _Left.Get() == _Right.Get();
}
template<class _Ty1, class _Ty2>
bool operator!=(const EnginePtr<_Ty1> _Left, const EnginePtr<_Ty2> _Right)
{
	return _Left.Get() != _Right.Get();
}