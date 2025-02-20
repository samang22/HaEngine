#pragma once
#include "CoreTypes.h"

/**
 * 값을 가장 가까운 상위의 'Alignment' 배수로 정렬합니다. 'Alignment'는 2의 제곱이어야 합니다.
 *
 * @param  Val        정렬할 값.
 * @param  Alignment  정렬 값, 2의 제곱이어야 합니다.
 *
 * @return 지정된 정렬에 맞춰 정렬된 값.
 */
template <typename T>
FORCEINLINE constexpr T Align(T Val, uint64 Alignment)
{
    //static_assert(TIsIntegral<T>::Value || TIsPointer<T>::Value, "Align expects an integer or pointer type");

    return (T)(((uint64)Val + Alignment - 1) & ~(Alignment - 1));
}
