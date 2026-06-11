#pragma once

#include <type_traits>
#include <concepts>
#include "Traits.hpp"

namespace Core
{
    namespace Detail
    {
        template<typename R, bool IsNoexcept, typename... Args>
        constexpr R CallableRefCallerWrapperFunctionPointer(const void* memory, Args... args) noexcept(IsNoexcept)
        {
            return (*reinterpret_cast<R (*)(Args...)>(const_cast<void*>(memory)))(std::forward<Args>(args)...);
        }

        //C: const, Member: const
        //C: non-const, Member: non-const
        //C: non-const, Member: const -> make C as const
        template<typename C, auto Member, typename R, bool IsNoexcept, typename... Args>
        constexpr R CallableRefCallerWrapperClassMember(const void* memory, Args... args) noexcept(IsNoexcept)
        {
            if constexpr(std::is_const_v<C>)
                return (reinterpret_cast<C*>(memory)->*Member)(std::forward<Args>(args)...);
            else
                return (reinterpret_cast<C*>(const_cast<void*>(memory))->*Member)(std::forward<Args>(args)...);
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

            template<typename C, auto Member = static_cast<R (std::remove_cvref_t<C>::*)(Args...) const noexcept(IsNoexcept)>(&std::remove_cvref_t<C>::operator())>
            requires IsConst && requires(C&& obj, Args... args) {
                { (std::forward<C>(obj).*Member)(std::forward<Args>(args)...) } -> std::same_as<R>;
            }
            CallableRefBase(C&& obj, NonType<Member> = NonTypeArgument<Member>) noexcept
                : memory(reinterpret_cast<const void*>(std::addressof(obj))),
                  caller(&Detail::CallableRefCallerWrapperClassMember<const std::remove_reference_t<C>, Member, R, IsNoexcept>)
            {}

            template<typename C, auto Member = static_cast<R (std::remove_cvref_t<C>::*)(Args...) noexcept(IsNoexcept)>(&std::remove_cvref_t<C>::operator())>
            requires(!IsConst && !std::is_const_v<C>) && requires(C&& obj, Args... args) {
                { (std::forward<C>(obj).*Member)(std::forward<Args>(args)...) } -> std::same_as<R>;
            }
            CallableRefBase(C&& obj, NonType<Member> = NonTypeArgument<Member>) noexcept
                : memory(reinterpret_cast<const void*>(std::addressof(obj))),
                  caller(&Detail::CallableRefCallerWrapperClassMember<std::remove_reference_t<C>, Member, R, IsNoexcept>)
            {}

            ~CallableRefBase() = default;
            CallableRefBase(const CallableRefBase&) = default;
            CallableRefBase(CallableRefBase&&) = default;
            CallableRefBase& operator=(const CallableRefBase&) = default;
            CallableRefBase& operator=(CallableRefBase&&) = default;

            R operator()(Args... args) const noexcept(IsNoexcept)
            {
                return this->caller(this->memory, std::forward<Args>(args)...);
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

    template<typename C>
    CallableRef(C&& obj) -> CallableRef<ClassMemberType<decltype(&std::remove_cvref_t<C>::operator())>>;

    template<auto Member, typename R, typename... Args, typename C>
    CallableRef(C&& obj, NonType<Member, R (std::remove_cvref_t<C>::*)(Args...)>) -> CallableRef<R(Args...)>;

    template<auto Member, typename R, typename... Args, typename C>
    CallableRef(C&& obj, NonType<Member, R (std::remove_cvref_t<C>::*)(Args...) const>) -> CallableRef<R(Args...) const>;

    template<auto Member, typename R, typename... Args, typename C>
    CallableRef(C&& obj, NonType<Member, R (std::remove_cvref_t<C>::*)(Args...) noexcept>) -> CallableRef<R(Args...) noexcept>;

    template<auto Member, typename R, typename... Args, typename C>
    CallableRef(C&& obj, NonType<Member, R (std::remove_cvref_t<C>::*)(Args...) const noexcept>) -> CallableRef<R(Args...) const noexcept>;
};