#pragma once

#include <memory>

namespace Core
{
    namespace Detail
    {
        template<typename F, typename... Args>
        constexpr auto CallableRefCallerWrapper(void* memory, Args... args) noexcept(
            noexcept((*reinterpret_cast<F*>(memory))(args...)))
        {
            return (*reinterpret_cast<F*>(memory))(args...);
        }

        template<typename F, typename... Args>
        constexpr auto CallableRefCallerWrapper(const void* memory, Args... args) noexcept(
            noexcept((*reinterpret_cast<const F*>(memory))(args...)))
        {
            return (*reinterpret_cast<const F*>(memory))(args...);
        }
    };

    template<typename F>
    requires std::is_function_v<F>
    class CallableRef;

    template<typename R, typename... Args>
    class CallableRef<R(Args...)>
    {
    public:
        CallableRef()
            : memory(nullptr),
              caller(nullptr)
        {}

        template<typename F>
        requires requires(F& func, Args... args) {
            { std::invoke(func, args...) } -> std::same_as<R>;
        }
        CallableRef(F& func) noexcept
            : memory(reinterpret_cast<void*>(std::addressof(func))),
              caller(&Detail::CallableRefCallerWrapper<F, Args...>)
        {}

        ~CallableRef() = default;
        CallableRef(const CallableRef&) = default;
        CallableRef(CallableRef&&) = default;
        CallableRef& operator=(const CallableRef&) = default;
        CallableRef& operator=(CallableRef&&) = default;

        template<typename F>
        requires requires(F& func, Args... args) {
            { std::invoke(func, args...) } -> std::same_as<R>;
        }
        CallableRef& operator=(F& func) noexcept
        {
            this->memory = reinterpret_cast<void*>(std::addressof(func));
            this->caller = &Detail::CallableRefCallerWrapper<F, Args...>;

            return *this;
        }

        R operator()(Args... args) const
        {
            return this->caller(this->memory, args...);
        }

        constexpr explicit operator bool() const noexcept
        {
            return this->caller != nullptr;
        }
    private:
        void* memory;
        R (*caller)(void* memory, Args...);
    };

    template<typename R, typename... Args>
    class CallableRef<R(Args...) const>
    {
    public:
        CallableRef()
            : memory(nullptr),
              caller(nullptr)
        {}

        template<typename F>
        requires requires(const F& func, Args... args) {
            { std::invoke(func, args...) } -> std::same_as<R>;
        }
        CallableRef(const F& func) noexcept
            : memory(reinterpret_cast<const void*>(std::addressof(func))),
              caller(&Detail::CallableRefCallerWrapper<F, Args...>)
        {}

        ~CallableRef() = default;
        CallableRef(const CallableRef&) = default;
        CallableRef(CallableRef&&) = default;
        CallableRef& operator=(const CallableRef&) = default;
        CallableRef& operator=(CallableRef&&) = default;

        template<typename F>
        requires requires(const F& func, Args... args) {
            { std::invoke(func, args...) } -> std::same_as<R>;
        }
        CallableRef& operator=(const F& func) noexcept
        {
            this->memory = reinterpret_cast<const void*>(std::addressof(func));
            this->caller = &Detail::CallableRefCallerWrapper<F, Args...>;

            return *this;
        }

        R operator()(Args... args) const
        {
            return this->caller(this->memory, args...);
        }

        constexpr explicit operator bool() const noexcept
        {
            return this->caller != nullptr;
        }
    private:
        const void* memory;
        R (*caller)(const void* memory, Args...);
    };

    template<typename R, typename... Args>
    class CallableRef<R(Args...) noexcept>
    {
    public:
        CallableRef()
            : memory(nullptr),
              caller(nullptr)
        {}

        template<typename F>
        requires requires(F& func, Args... args) {
            { std::invoke(func, args...) } noexcept -> std::same_as<R>;
        }
        CallableRef(F& func) noexcept
            : memory(reinterpret_cast<void*>(std::addressof(func))),
              caller(&Detail::CallableRefCallerWrapper<F, Args...>)
        {}

        ~CallableRef() = default;
        CallableRef(const CallableRef&) = default;
        CallableRef(CallableRef&&) = default;
        CallableRef& operator=(const CallableRef&) = default;
        CallableRef& operator=(CallableRef&&) = default;

        template<typename F>
        requires requires(F& func, Args... args) {
            { std::invoke(func, args...) } noexcept -> std::same_as<R>;
        }
        CallableRef& operator=(F& func) noexcept
        {
            this->memory = reinterpret_cast<void*>(std::addressof(func));
            this->caller = &Detail::CallableRefCallerWrapper<F, Args...>;

            return *this;
        }

        R operator()(Args... args) const noexcept
        {
            return this->caller(this->memory, args...);
        }

        constexpr explicit operator bool() const noexcept
        {
            return this->caller != nullptr;
        }
    private:
        void* memory;
        R (*caller)(void* memory, Args...) noexcept;
    };

    template<typename R, typename... Args>
    class CallableRef<R(Args...) const noexcept>
    {
    public:
        CallableRef()
            : memory(nullptr),
              caller(nullptr)
        {}

        template<typename F>
        requires requires(const F& func, Args... args) {
            { std::invoke(func, args...) } noexcept -> std::same_as<R>;
        }
        CallableRef(const F& func) noexcept
            : memory(reinterpret_cast<const void*>(std::addressof(func))),
              caller(&Detail::CallableRefCallerWrapper<F, Args...>)
        {}

        ~CallableRef() = default;
        CallableRef(const CallableRef&) = default;
        CallableRef(CallableRef&&) = default;
        CallableRef& operator=(const CallableRef&) = default;
        CallableRef& operator=(CallableRef&&) = default;

        template<typename F>
        requires requires(const F& func, Args... args) {
            { std::invoke(func, args...) } noexcept -> std::same_as<R>;
        }
        CallableRef& operator=(const F& func) noexcept
        {
            this->memory = reinterpret_cast<const void*>(std::addressof(func));
            this->caller = &Detail::CallableRefCallerWrapper<F, Args...>;

            return *this;
        }

        R operator()(Args... args) const noexcept
        {
            return this->caller(this->memory, args...);
        }

        constexpr explicit operator bool() const noexcept
        {
            return this->caller != nullptr;
        }
    private:
        const void* memory;
        R (*caller)(const void* memory, Args...) noexcept;
    };
};