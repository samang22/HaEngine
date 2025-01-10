#pragma once
#include "CoreTypes.h"

#define UE_SMALL_NUMBER			(1.e-8f)


class FMath
{
public:
    static bool IsNearlyEqual(float a, float b, float epsilon = UE_SMALL_NUMBER)
    {
        return std::fabs(a - b) <= epsilon * max(std::fabs(a), std::fabs(b));
    }

    static float Fmod(float X, float Y)
    {
        const float AbsY = std::abs(Y);
        if (AbsY <= UE_SMALL_NUMBER) // Note: this constant should match that used by VectorMod() implementations
        {
            _ASSERT(false);
            return 0.0;
        }

        return fmodf(X, Y);
    }

    template <typename T>
    [[nodiscard]] static constexpr FORCEINLINE T Modulo(T Value, T Base)
    {
        if constexpr (std::is_floating_point_v<T>)
        {
            return FMath::Fmod(Value, Base);
        }
        else
        {
            return Value % Base;
        }
    }

    template<class T>
    static constexpr FORCEINLINE T Wrap(const T X, const T Min, const T Max)
    {
        T Size = Max - Min;
        if (Size == 0)
        {
            return Max;
        }

        T EndVal = X;
        if (EndVal < Min)
        {
            T Mod = FMath::Modulo(Min - EndVal, Size);
            EndVal = (Mod != (T)0) ? Max - Mod : Min;
        }
        else if (EndVal > Max)
        {
            T Mod = FMath::Modulo(EndVal - Max, Size);
            EndVal = (Mod != (T)0) ? Min + Mod : Max;
        }
        return EndVal;
    }
};