#pragma once

#include <type_traits>
#include <concepts>
#include "NonType.hpp"

namespace Core
{
    namespace Detail
    {
        template<typename R, bool IsNoexcept, typename... Args>
        constexpr R CallableRefCallerWrapperFunctionPointer(const void* memory, Args... args) noexcept(IsNoexcept)
        {
            return (*reinterpret_cast<R (*)(Args...)>(const_cast<void*>(memory)))(args...);
        }

        template<typename T, typename R, bool IsNoexcept, typename... Args>
        constexpr R CallableRefCallerWrapperObject(const void* memory, Args... args) noexcept(IsNoexcept)
        {
            if constexpr(std::is_const_v<T>)
                return (*reinterpret_cast<T*>(memory))(args...);
            else
                return (*reinterpret_cast<T*>(const_cast<void*>(memory)))(args...);
        }

        //C: const, Member: const
        //C: non-const, Member: non-const
        //C: non-const, Member: const -> make C as const
        template<typename C, auto Member, typename R, bool IsNoexcept, typename... Args>
        constexpr R CallableRefCallerWrapperClassMember(const void* memory, Args... args) noexcept(IsNoexcept)
        {
            if constexpr(std::is_const_v<C>)
                return (reinterpret_cast<C*>(memory)->*Member)(args...);
            else
                return (reinterpret_cast<C*>(const_cast<void*>(memory))->*Member)(args...);
        }

        template<typename R, bool IsConst, bool IsNoexcept, typename... Args>
        class CallableRefBase
        {
        public:
            CallableRefBase()
                : memory(nullptr),
                  caller(nullptr)
            {}

            CallableRefBase(R (*func_ptr)(Args...) noexcept(IsNoexcept)) noexcept
            requires(!IsConst)
                : memory(reinterpret_cast<const void*>(func_ptr)),
                  caller(&Detail::CallableRefCallerWrapperFunctionPointer<R, IsNoexcept, Args...>)
            {}

            template<typename T>
            requires((!std::is_const_v<T>) || (std::is_const_v<T> && IsConst)) && requires(T& obj, Args... args) {
                { obj(args...) } -> std::same_as<R>;
            }
            CallableRefBase(T&& obj) noexcept
                : memory(reinterpret_cast<const void*>(std::addressof(obj))),
                  caller(&Detail::CallableRefCallerWrapperObject<std::conditional_t<IsConst, const std::remove_reference_t<T>, std::remove_reference_t<T>>, R, IsNoexcept>)
            {}

            template<auto Member, typename C>
            requires((!std::is_const_v<C>) || (std::is_const_v<C> && IsConst)) && requires(C& obj, Args... args) {
                { (obj.*Member)(args...) } -> std::same_as<R>;
            }
            CallableRefBase(NonTypeArgument<Member>, C&& obj) noexcept
                : memory(reinterpret_cast<const void*>(std::addressof(obj))),
                  caller(&Detail::CallableRefCallerWrapperClassMember<std::conditional_t<IsConst, const std::remove_reference_t<C>, std::remove_reference_t<C>>, Member, R, IsNoexcept>)
            {}

            ~CallableRefBase() = default;
            CallableRefBase(const CallableRefBase&) = default;
            CallableRefBase(CallableRefBase&&) = default;
            CallableRefBase& operator=(const CallableRefBase&) = default;
            CallableRefBase& operator=(CallableRefBase&&) = default;

            template<typename... NArgs>
            requires std::invocable<R(Args...), NArgs...>
            R operator()(NArgs&&... args) const noexcept(IsNoexcept)
            {
                return this->caller(this->memory, std::forward<NArgs>(args)...);
            }

            constexpr explicit operator bool() const noexcept
            {
                return this->caller != nullptr;
            }
        private:
            const void* memory;
            R (*caller)(const void* memory, Args... args) noexcept(IsNoexcept);
        };
    };

    template<typename F>
    requires std::is_function_v<F>
    class CallableRef;

    template<typename R, typename... Args>
    class CallableRef<R(Args...)> : public Detail::CallableRefBase<R, false, false, Args...>
    {
        using Detail::CallableRefBase<R, false, false, Args...>::CallableRefBase;
    };

    template<typename R, typename... Args>
    class CallableRef<R(Args...) const> : public Detail::CallableRefBase<R, true, false, Args...>
    {
        using Detail::CallableRefBase<R, true, false, Args...>::CallableRefBase;
    };

    template<typename R, typename... Args>
    class CallableRef<R(Args...) noexcept> : public Detail::CallableRefBase<R, false, true, Args...>
    {
        using Detail::CallableRefBase<R, false, true, Args...>::CallableRefBase;
    };

    template<typename R, typename... Args>
    class CallableRef<R(Args...) const noexcept> : public Detail::CallableRefBase<R, true, true, Args...>
    {
        using Detail::CallableRefBase<R, true, true, Args...>::CallableRefBase;
    };

    template<typename R, typename... Args>
    CallableRef(R (*func_ptr)(Args...)) -> CallableRef<R(Args...)>;

    template<typename R, typename... Args>
    CallableRef(R (*func_ptr)(Args...) noexcept) -> CallableRef<R(Args...) noexcept>;
};