#pragma once

#include "Variadic.hpp"

namespace Core
{
    namespace Detail
    {
        template<typename T>
        struct CallableTraits
        {
            constexpr static Bool IsCallable = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...)>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = false;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) const>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = true;
            constexpr static Bool IsVolatile = false;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) volatile>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...)&>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = false;
            constexpr static Bool IsLValueReferenceQualified = true;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) &&>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = false;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = true;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) noexcept>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = false;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = true;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) const volatile>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = true;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) const&>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = true;
            constexpr static Bool IsVolatile = false;
            constexpr static Bool IsLValueReferenceQualified = true;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) const&&>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = true;
            constexpr static Bool IsVolatile = false;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = true;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) const noexcept>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = true;
            constexpr static Bool IsVolatile = false;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = true;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) const volatile&>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = true;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = true;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) const volatile&&>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = true;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = true;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) const volatile noexcept>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = true;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = true;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) const volatile & noexcept>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = true;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = true;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = true;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) const volatile && noexcept>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = true;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = true;
            constexpr static Bool IsNoexcept = true;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) volatile&>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = true;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) volatile&&>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = true;
            constexpr static Bool IsNoexcept = false;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) volatile noexcept>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = true;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) volatile & noexcept>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = true;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = true;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) volatile && noexcept>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = true;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = true;
            constexpr static Bool IsNoexcept = true;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) & noexcept>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = false;
            constexpr static Bool IsLValueReferenceQualified = true;
            constexpr static Bool IsRValueReferenceQualified = false;
            constexpr static Bool IsNoexcept = true;
        };

        template<typename R, typename... Args>
        struct CallableTraits<R(Args...) && noexcept>
        {
            constexpr static Bool IsCallable = true;

            using ReturnType = R;
            using Arguments = Variadic<Args...>;
            constexpr static Bool IsConstant = false;
            constexpr static Bool IsVolatile = false;
            constexpr static Bool IsLValueReferenceQualified = false;
            constexpr static Bool IsRValueReferenceQualified = true;
            constexpr static Bool IsNoexcept = true;
        };

        template<typename T>
        struct MemberCallableImpl
        {
            constexpr static Bool Value = false;
        };

        template<typename C, typename M>
        struct MemberCallableImpl<M C::*>
        {
            constexpr static Bool Value = CallableTraits<M>::IsCallable;
        };
    };

    template<typename T>
    concept Callable = Detail::CallableTraits<T>::IsCallable;

    template<typename T>
    concept MemberCallable = Detail::MemberCallableImpl<T>::Value;

    template<typename T>
    concept ConstQualifiedCallable = Detail::CallableTraits<T>::IsConstant;

    template<typename T>
    concept VolatileQualifiedCallable = Detail::CallableTraits<T>::IsVolatile;

    template<typename T>
    concept LValueReferenceQualifiedCallable = Detail::CallableTraits<T>::IsLValueReferenceQualified;

    template<typename T>
    concept RValueReferenceQualifiedCallable = Detail::CallableTraits<T>::IsRValueReferenceQualified;

    template<typename T>
    concept NoexceptQualifiedCallable = Detail::CallableTraits<T>::IsNoexcept;

    template<typename T>
    using CallableReturnType = Detail::CallableTraits<T>::ReturnType;

    template<typename T>
    using CallableArguments = Detail::CallableTraits<T>::Arguments;
};