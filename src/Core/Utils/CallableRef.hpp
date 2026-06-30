#pragma once

#include "Traits.hpp"
#include "Utility.hpp"
#include "CallableTraits.hpp"

namespace Core
{
    namespace Detail
    {
        template<typename R, Bool IsNoexcept, typename... Args>
        constexpr R CallableRefCallerWrapperFunctionPointer(const Void* memory, Args... args) noexcept(IsNoexcept)
        {
            return (*reinterpret_cast<R (*)(Args...)>(const_cast<Void*>(memory)))(Forward(args)...);
        }

        //C: const, Member: const
        //C: non-const, Member: non-const
        //C: non-const, Member: const -> make C as const
        template<typename C, auto Member, typename R, Bool IsNoexcept, typename... Args>
        constexpr R CallableRefCallerWrapperClassMember(const Void* memory, Args... args) noexcept(IsNoexcept)
        {
            if constexpr(Const<C>)
                return (reinterpret_cast<C*>(memory)->*Member)(Forward(args)...);
            else
                return (reinterpret_cast<C*>(const_cast<Void*>(memory))->*Member)(Forward(args)...);
        }

        template<typename R, Bool IsConst, Bool IsNoexcept, typename... Args>
        class CallableRefBase
        {
        public:
            CallableRefBase()
                : memory(nullptr),
                  caller(nullptr)
            {}

            CallableRefBase(R (*func_ptr)(Args...) noexcept(IsNoexcept)) noexcept
            requires(!IsConst)
                : memory(reinterpret_cast<const Void*>(func_ptr)),
                  caller(&Detail::CallableRefCallerWrapperFunctionPointer<R, IsNoexcept, Args...>)
            {}

            template<typename C, auto Member = static_cast<R (DropConstVolatileReference<C>::*)(Args...) const noexcept(IsNoexcept)>(&DropConstVolatileReference<C>::operator())>
            requires IsConst && requires(C&& obj, Args... args) {
                { (Forward(obj).*Member)(Forward(args)...) } -> SameAs<R>;
            }
            CallableRefBase(C&& obj, NonType<Member> = NonTypeArgument<Member>) noexcept
                : memory(reinterpret_cast<const Void*>(GetAddress(obj))),
                  caller(&Detail::CallableRefCallerWrapperClassMember<const DropConstVolatileReference<C>, Member, R, IsNoexcept>)
            {}

            template<typename C, auto Member = static_cast<R (DropConstVolatileReference<C>::*)(Args...) noexcept(IsNoexcept)>(&DropConstVolatileReference<C>::operator())>
            requires(!IsConst && !Const<C>) && requires(C&& obj, Args... args) {
                { (Forward(obj).*Member)(Forward(args)...) } -> SameAs<R>;
            }
            CallableRefBase(C&& obj, NonType<Member> = NonTypeArgument<Member>) noexcept
                : memory(reinterpret_cast<const Void*>(GetAddress(obj))),
                  caller(&Detail::CallableRefCallerWrapperClassMember<DropConstVolatileReference<C>, Member, R, IsNoexcept>)
            {}

            ~CallableRefBase() = default;
            CallableRefBase(const CallableRefBase&) = default;
            CallableRefBase(CallableRefBase&&) = default;
            CallableRefBase& operator=(const CallableRefBase&) = default;
            CallableRefBase& operator=(CallableRefBase&&) = default;

            R operator()(Args... args) const noexcept(IsNoexcept)
            {
                return this->caller(this->memory, Forward(args)...);
            }

            constexpr explicit operator Bool() const noexcept
            {
                return this->caller != nullptr;
            }
        private:
            const Void* memory;
            R (*caller)(const Void* memory, Args... args) noexcept(IsNoexcept);
        };
    };

    template<typename F>
    requires Callable<F>
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
    CallableRef(C&& obj) -> CallableRef<ClassMemberType<decltype(&DropConstVolatileReference<C>::operator())>>;

    template<auto Member, typename R, typename... Args, typename C>
    CallableRef(C&& obj, NonType<Member, R (DropConstVolatileReference<C>::*)(Args...)>) -> CallableRef<R(Args...)>;

    template<auto Member, typename R, typename... Args, typename C>
    CallableRef(C&& obj, NonType<Member, R (DropConstVolatileReference<C>::*)(Args...) const>) -> CallableRef<R(Args...) const>;

    template<auto Member, typename R, typename... Args, typename C>
    CallableRef(C&& obj, NonType<Member, R (DropConstVolatileReference<C>::*)(Args...) noexcept>) -> CallableRef<R(Args...) noexcept>;

    template<auto Member, typename R, typename... Args, typename C>
    CallableRef(C&& obj, NonType<Member, R (DropConstVolatileReference<C>::*)(Args...) const noexcept>) -> CallableRef<R(Args...) const noexcept>;
};