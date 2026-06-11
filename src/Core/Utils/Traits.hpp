#pragma once

namespace Core
{
    //NonType
    template<auto Value, typename T = decltype(Value)>
    struct NonType
    {
        constexpr NonType() = default;
    };

    template<auto Value, typename T = decltype(Value)>
    constexpr inline NonType<Value> NonTypeArgument{};

    template<bool Condition, auto TrueValue, auto FalseValue>
    struct Conditional;

    template<auto TrueValue, auto FalseValue>
    struct Conditional<true, TrueValue, FalseValue>
    {
        constexpr static auto Value = TrueValue;
    };

    template<auto TrueValue, auto FalseValue>
    struct Conditional<false, TrueValue, FalseValue>
    {
        constexpr static auto Value = FalseValue;
    };

    template<bool Condition, auto TrueValue, auto FalseValue>
    constexpr inline auto ConditionalValue = Conditional<Condition, TrueValue, FalseValue>::Value;

    template<typename M>
    struct ClassMember;

    template<typename C, typename M>
    struct ClassMember<M C::*>
    {
        using Type = M;
    };

    template<typename T>
    using ClassMemberType = ClassMember<T>::Type;
};