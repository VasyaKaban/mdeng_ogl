#pragma once

#include "Types.hpp"

namespace Core
{
    //NonType
    template<auto Value, typename T = decltype(Value)>
    struct NonType
    {};

    template<auto Value>
    constexpr inline NonType<Value> NonTypeArgument{};

    //InPlaceType
    template<typename T>
    struct InPlaceType
    {
        using Type = T;
    };

    template<typename T>
    constexpr inline InPlaceType<T> InPlaceTypeArgument{};

    namespace Detail
    {
        template<typename M>
        struct ClassMemberImpl
        {
            constexpr static Bool Value = false;
        };

        template<typename C, typename M>
        struct ClassMemberImpl<M C::*>
        {
            constexpr static Bool Value = true;
        };

        template<typename M>
        struct ClassMemberTypeImpl;

        template<typename C, typename M>
        struct ClassMemberTypeImpl<M C::*>
        {
            using Type = M;
        };

        template<typename M>
        struct ClassMemberClassTypeImpl;

        template<typename C, typename M>
        struct ClassMemberClassTypeImpl<M C::*>
        {
            using Type = C;
        };

        template<Bool Condition, typename TrueType, typename FalseType>
        struct ConditionalImpl
        {
            using Type = TrueType;
        };

        template<typename TrueType, typename FalseType>
        struct ConditionalImpl<false, TrueType, FalseType>
        {
            using Type = FalseType;
        };

        template<typename T, typename U>
        struct SameAsImpl
        {
            constexpr static Bool Value = false;
        };

        template<typename T>
        struct SameAsImpl<T, T>
        {
            constexpr static Bool Value = true;
        };

        template<typename T>
        struct RValueReferenceImpl
        {
            constexpr static Bool Value = false;
        };

        template<typename T>
        struct RValueReferenceImpl<T&&>
        {
            constexpr static Bool Value = true;
        };

        template<typename T>
        struct LValueReferenceImpl
        {
            constexpr static Bool Value = false;
        };

        template<typename T>
        struct LValueReferenceImpl<T&>
        {
            constexpr static Bool Value = true;
        };

        template<typename T>
        struct ReferenceImpl
        {
            constexpr static Bool Value = RValueReferenceImpl<T>::Value || LValueReferenceImpl<T>::Value;
        };

        template<typename T>
        struct ConstImpl
        {
            constexpr static Bool Value = false;
        };

        template<typename T>
        struct ConstImpl<const T>
        {
            constexpr static Bool Value = true;
        };

        template<typename T>
        struct VolatileImpl
        {
            constexpr static Bool Value = false;
        };

        template<typename T>
        struct VolatileImpl<volatile T>
        {
            constexpr static Bool Value = true;
        };

        template<typename T>
        struct DropRValueReferenceImpl
        {
            using Type = T;
        };

        template<typename T>
        struct DropRValueReferenceImpl<T&&>
        {
            using Type = T;
        };

        template<typename T>
        struct DropLValueReferenceImpl
        {
            using Type = T;
        };

        template<typename T>
        struct DropLValueReferenceImpl<T&>
        {
            using Type = T;
        };

        template<typename T>
        struct DropReferenceImpl
        {
            using Type = DropRValueReferenceImpl<typename DropLValueReferenceImpl<T>::Type>::Type;
        };

        template<typename T>
        struct DropConstImpl
        {
            using Type = T;
        };

        template<typename T>
        struct DropConstImpl<const T>
        {
            using Type = T;
        };

        template<typename T>
        struct DropVolatileImpl
        {
            using Type = T;
        };

        template<typename T>
        struct DropVolatileImpl<volatile T>
        {
            using Type = T;
        };

        template<typename T>
        struct DropConstVolatileReferenceImpl
        {
            using Type = DropReferenceImpl<typename DropVolatileImpl<typename DropConstImpl<T>::Type>::Type>::Type;
        };

        template<typename T>
        struct PointerImpl
        {
            constexpr static Bool Value = false;
        };

        template<typename T>
        struct PointerImpl<T*>
        {
            constexpr static Bool Value = true;
        };

        template<typename T>
        struct ArrayImpl
        {
            constexpr static Bool Value = false;
        };

        template<typename T>
        struct ArrayImpl<T[]>
        {
            constexpr static Bool Value = true;
        };

        template<typename T, template<typename...> typename D>
        struct TypeInstantiationImpl
        {
            constexpr static Bool Value = false;
        };

        template<typename... Args, template<typename...> typename D>
        struct TypeInstantiationImpl<D<Args...>, D>
        {
            constexpr static Bool Value = true;
        };

        template<typename T, template<auto...> typename D>
        struct NonTypeInstantiationImpl
        {
            constexpr static Bool Value = false;
        };

        template<auto... Args, template<auto...> typename D>
        struct NonTypeInstantiationImpl<D<Args...>, D>
        {
            constexpr static Bool Value = true;
        };
    };

    template<typename T>
    consteval T&& DeclareValue() noexcept
    {
        static_assert("Do not use in evaluated context");
    }

    template<typename M>
    concept ClassMember = Detail::ClassMemberImpl<M>::Value;

    template<ClassMember M>
    using ClassMemberType = Detail::ClassMemberTypeImpl<M>::Type;

    template<ClassMember M>
    using ClassMemberClassType = Detail::ClassMemberClassTypeImpl<M>::Type;

    template<Bool Condition, typename TrueType, typename FalseType>
    using Conditional = Detail::ConditionalImpl<Condition, TrueType, FalseType>::Type;

    template<typename T, typename U>
    concept SameAs = Detail::SameAsImpl<T, U>::Value;

    template<typename T>
    concept RValueReference = Detail::RValueReferenceImpl<T>::Value;

    template<typename T>
    concept LValueReference = Detail::LValueReferenceImpl<T>::Value;

    template<typename T>
    concept Reference = Detail::ReferenceImpl<T>::Value;

    template<typename T>
    concept Const = Detail::ConstImpl<T>::Value;

    template<typename T>
    concept Volatile = Detail::VolatileImpl<T>::Value;

    template<typename T>
    using DropRValueReference = Detail::DropRValueReferenceImpl<T>;

    template<typename T>
    using DropLValueReference = Detail::DropLValueReferenceImpl<T>;

    template<typename T>
    using DropReference = Detail::DropReferenceImpl<T>::Type;

    template<typename T>
    using DropConst = Detail::DropConstImpl<T>;

    template<typename T>
    using DropVolatile = Detail::DropVolatileImpl<T>;

    template<typename T>
    using DropConstVolatileReference = Detail::DropConstVolatileReferenceImpl<T>::Type;

    template<typename T>
    concept Destructible = requires(T value) { value.~T(); };

    template<typename T>
    concept NoexceptDestructible = requires(T value) {
        { value.~T() } noexcept;
    };

    template<typename T>
    concept DefaultConstructible = requires { T(); };

    template<typename T>
    concept NoexceptDefaultConstructible = requires {
        { T() } noexcept;
    };

    template<typename T>
    concept CopyConstructible = requires(const T& value) { T(value); };

    template<typename T>
    concept NoexceptCopyConstructible = requires(const T& value) {
        { T(value) } noexcept;
    };

    template<typename T>
    concept MoveConstructible = requires(T value) { T(static_cast<T&&>(value)); };

    template<typename T>
    concept NoexceptMoveConstructible = requires(T value) {
        { T(static_cast<T&&>(value)) } noexcept;
    };

    template<typename T>
    concept CopyAssignable = requires(T& value1, const T& value2) { value1 = value2; };

    template<typename T>
    concept NoexceptCopyAssignable = requires(T& value1, const T& value2) {
        { value1 = value2 } noexcept;
    };

    template<typename T>
    concept MoveAssignable = requires(T& value1, T value2) { value1 = static_cast<T&&>(value2); };

    template<typename T>
    concept NoexceptMoveAssignable = requires(T& value1, T value2) {
        { value1 = static_cast<T&&>(value2) } noexcept;
    };

    template<typename T, typename... Args>
    concept Constructible = requires(Args... args) { T(args...); };

    template<typename T, typename... Args>
    concept NoexceptConstructible = requires(Args... args) {
        { T(args...) } noexcept;
    };

    template<typename T, typename U>
    concept Assignable = requires(T& value1, U value2) { value1 = value2; };

    template<typename T, typename U>
    concept NoexceptAssignable = requires(T& value1, U value2) {
        { value1 = value2 } noexcept;
    };

    template<typename T>
    concept Pointer = Detail::PointerImpl<T>::Value;

    template<typename T>
    concept Array = Detail::ArrayImpl<T>::Value;

    template<typename T, template<typename...> typename D>
    concept TypeInstantiation = Detail::TypeInstantiationImpl<T, D>::Value;

    template<typename T, template<auto...> typename D>
    concept NonTypeInstantiation = Detail::NonTypeInstantiationImpl<T, D>::Value;

    template<typename F, typename... Args>
    concept Invocable = (LValueReference<F> && requires(F& func, Args... args) { func(args...); }) || (!LValueReference<F> && requires(F&& func, Args... args) { static_cast<F&&>(func)(args...); });

    template<typename F, typename... Args>
    concept NoexceptInvocable = (LValueReference<F> && requires(F& func, Args... args) {
                                    { func(args...) } noexcept;
                                }) || (!LValueReference<F> && requires(F&& func, Args... args) {
                                    { static_cast<F&&>(func)(args...) } noexcept;
                                });

    template<typename F, typename R, typename... Args>
    concept InvocableWithResult = (LValueReference<F> && requires(F& func, Args... args) {
                                      { func(args...) } -> SameAs<R>;
                                  }) || (!LValueReference<F> && requires(F&& func, Args... args) {
                                      { static_cast<F&&>(func)(args...) } -> SameAs<R>;
                                  });

    template<typename F, typename R, typename... Args>
    concept NoexceptInvocableWithResult = (LValueReference<F> && requires(F& func, Args... args) {
                                              { func(args...) } noexcept -> SameAs<R>;
                                          }) || (!LValueReference<F> && requires(F&& func, Args... args) {
                                              { static_cast<F&&>(func)(args...) } noexcept -> SameAs<R>;
                                          });

    template<typename F, typename... Args>
    using InvokeResult = decltype([](F&& func, Args&&...args){
        return Forward(func)(Forward(args)...);
    }());

    template<typename T>
    concept FloatingPoint = SameAs<T, Float32> || SameAs<T, Float64>;

    template<typename T>
    concept Integral = SameAs<T, Int8> || SameAs<T, Int16> || SameAs<T, Int32> || SameAs<T, Int64> || SameAs<T, UInt8> || SameAs<T, UInt16> || SameAs<T, UInt32> || SameAs<T, UInt64> ||
                       SameAs<T, UTF8Char> || SameAs<T, UTF16Char> || SameAs<T, UTF32Char> || SameAs<T, Char> || SameAs<T, WideChar> || SameAs<T, Bool>;

    template<typename T>
    concept SignedIntegral = Integral<T> && (T(-1) < T(0));

    template<typename T>
    concept UnsignedIntegral = Integral<T> && !SignedIntegral<T>;

    template<typename T>
    concept Arithmetic = FloatingPoint<T> || Integral<T>;

    template<Integral I>
    using MakeUnsignedIntegral = Conditional<sizeof(I) == 8, UInt8, Conditional<sizeof(I) == 16, UInt16, Conditional<sizeof(I) == 32, UInt32, UInt64>>>;

    template<Integral I>
    using MakeSignedIntegral = Conditional<sizeof(I) == 8, Int8, Conditional<sizeof(I) == 16, Int16, Conditional<sizeof(I) == 32, Int32, Int64>>>;

    namespace Detail
    {
        template<typename T, typename... Types>
        struct CommonArithmeticImpl
        {};

        template<typename T>
        struct CommonArithmeticImpl<T>
        {
            using Type = T;
        };

        /*
        signed signed -> select bigger size
        usnigned unsigned -> select bigger size
        float float -> select bigger size
        */
        template<typename T, typename U, typename... Types>
        requires(SignedIntegral<T> && SignedIntegral<U>) || (UnsignedIntegral<T> && UnsignedIntegral<U>) || (FloatingPoint<T> && FloatingPoint<U>)
        struct CommonArithmeticImpl<T, U, Types...>
        {
            using Type = CommonArithmeticImpl<Conditional<sizeof(T) < sizeof(U), U, T>, Types...>::Type;
        };

        //signed unsigned if same size -> select unsigned else select bigger size
        template<typename T, typename U, typename... Types>
        requires(SignedIntegral<T> && UnsignedIntegral<U>) || (UnsignedIntegral<T> && SignedIntegral<U>)
        struct CommonArithmeticImpl<T, U, Types...>
        {
            using Type = CommonArithmeticImpl<Conditional<sizeof(T) == sizeof(U), ConditionalImpl<UnsignedIntegral<U>, U, T>, Conditional<sizeof(T) < sizeof(U), U, T>>, Types...>;
        };

        /*
        float signed -> float
        unsigned float -> float
        */
        template<typename T, typename U, typename... Types>
        requires FloatingPoint<T> || FloatingPoint<U>
        struct CommonArithmeticImpl<T, U, Types...>
        {
            using Type = CommonArithmeticImpl<Conditional<FloatingPoint<T>, T, U>, Types...>;
        };
    };

    template<Arithmetic... Types>
    requires(sizeof...(Types) != 0)
    using CommonArithmetic = Detail::CommonArithmeticImpl<Types...>::Type;

    template<typename T>
    concept StandardLayout = __is_standard_layout(T);

    template<typename B, typename D>
    concept BaseOf = __is_base_of(B, D);

    template<typename T>
    concept Complex = requires(int T::*) { true; };

    template<typename T, typename U>
    concept LessComparable = requires(T value1, U value2) {
        { value1 < value2 } noexcept -> SameAs<Bool>;
    };

    template<typename T, typename U>
    concept LessOrEqualComparable = requires(T value1, U value2) {
        { value1 <= value2 } noexcept -> SameAs<Bool>;
    };

    template<typename T, typename U>
    concept EqualComparable = requires(T value1, U value2) {
        { value1 == value2 } noexcept -> SameAs<Bool>;
    };

    template<typename T, typename U>
    concept GreaterComparable = requires(T value1, U value2) {
        { value1 > value2 } noexcept -> SameAs<Bool>;
    };

    template<typename T, typename U>
    concept GreaterOrEqualComparable = requires(T value1, U value2) {
        { value1 >= value2 } noexcept -> SameAs<Bool>;
    };

    template<typename T, typename U>
    concept NotEqualComparable = requires(T value1, U value2) {
        { value1 != value2 } noexcept -> SameAs<Bool>;
    };

    template<DeviceSize... Indices>
    struct IndexSequence
    {};

    namespace Detail
    {
        template<DeviceSize N, DeviceSize... Indices>
        struct MakeIndexSequenceImpl
        {
            using Type = MakeIndexSequenceImpl<N - 1, N - 1, Indices...>::Type;
        };

        template<DeviceSize... Indices>
        struct MakeIndexSequenceImpl<0, Indices...>
        {
            using Type = IndexSequence<Indices...>;
        };
    };

    template<DeviceSize N>
    using MakeIndexSequence = Detail::MakeIndexSequenceImpl<N>::Type;
};