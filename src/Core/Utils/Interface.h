#pragma once

#include <cstdint>
#include <utility>
#include "Core/API.h"

namespace Core
{
    using InterfaceVersion = uint32_t;

    class CORE_API Interface
    {
    public:
        virtual ~Interface() = 0;

        virtual InterfaceVersion GetVersion() const noexcept = 0;
        virtual void Acquire() noexcept = 0;
        virtual void Release() noexcept = 0;
    };

    template<typename T>
    class InterfacePointer
    {
    public:
        InterfacePointer(T* obj = nullptr) noexcept
            : obj(obj)
        {
            obj->Interface::Acquire();
        }

        ~InterfacePointer()
        {
            if(obj)
                obj->Interface::Release();
        }

        InterfacePointer(const InterfacePointer& ifacep) noexcept
        {
            ifacep.obj->Interface::Acquire();
            obj = ifacep.obj;
        }

        InterfacePointer(InterfacePointer&& ifacep) noexcept
            : obj(std::exchange(ifacep.obj, nullptr))
        {}

        InterfacePointer& operator=(const InterfacePointer& ifacep) noexcept
        {
            Reset();

            ifacep.obj->Interface::Acquire();
            obj = ifacep.obj;

            return *this;
        }

        InterfacePointer& operator=(InterfacePointer&& ifacep) noexcept
        {
            Reset();

            obj = std::exchange(ifacep.obj, nullptr);

            return *this;
        }

        template<typename U>
        requires std::convertible_to<U*, T*>
        InterfacePointer(const InterfacePointer<U>& ifacep) noexcept
        {
            ifacep.obj->Interface::Acquire();
            obj = ifacep.obj;
        }

        template<typename U>
        requires std::convertible_to<U*, T*>
        InterfacePointer(InterfacePointer<U>&& ifacep) noexcept
            : obj(std::exchange(ifacep.obj, nullptr))
        {}

        template<typename U>
        requires std::convertible_to<U*, T*>
        InterfacePointer& operator=(const InterfacePointer<U>& ifacep) noexcept
        {
            Reset();

            ifacep.obj->Interface::Acquire();
            obj = ifacep.obj;

            return *this;
        }

        template<typename U>
        requires std::convertible_to<U*, T*>
        InterfacePointer& operator=(InterfacePointer<U>&& ifacep) noexcept
        {
            Reset();

            obj = std::exchange(ifacep.obj, nullptr);

            return *this;
        }

        void Reset() noexcept
        {
            if(obj)
            {
                obj->Interface::Release();
                obj = nullptr;
            }
        }

        T* operator->() const noexcept
        {
            return obj;
        }

        explicit operator bool() const noexcept
        {
            return obj != nullptr;
        }
    private:
        T* obj;
    };
};