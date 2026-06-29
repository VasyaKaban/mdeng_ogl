#pragma once

#include "UUID.hpp"
#include "Traits.hpp"
#include "Core/API.h"

namespace Core
{
#define CORE_INTERFACE_ID(VALUE) constexpr static UUID INTERFACE_IDENTITY = VALUE;
#define CORE_INTERFACE_GET_ID(CLASS, ...) CLASS __VA_OPT__(, __VA_ARGS__)::INTERFACE_IDENTITY

    class CORE_API Interface
    {
    public:
        CORE_INTERFACE_ID("57e7739d-9466-4a43-8e02-c0ef30a14df9")

        virtual ~Interface() = 0;

        //Cast: check that current object implements(has in hierarchy class with current ClassID) and performs inner cast to the desired class type(with possible class disambiguation)
        //This methods should be implemented for interfaces and for classes(in case of ambiguation)
        virtual const void* Cast(const UUID& id) const noexcept;
        virtual void Acquire() noexcept = 0;
        virtual void Release() noexcept = 0;
    };

    //both To and From can be const or not
    template<typename To, typename From>
    requires BaseOf<Interface, DropConstVolatileReference<To>> && SameAs<DropConstVolatileReference<To>, To> && BaseOf<Interface, DropConstVolatileReference<From>> &&
             SameAs<DropConstVolatileReference<From>, From> && Constructible<To*, From*>
    To* InterfaceCast(From* from) noexcept
    {
        const auto& id = CORE_INTERFACE_GET_ID(DropConstVolatileReference<To>);

        if constexpr(SameAs<DropConstVolatileReference<From>, From>)
            return static_cast<To*>(const_cast<void*>(from->Cast(id)));
        else
            return static_cast<To*>(from->Cast(id));
    }

    template<typename T>
    class InterfacePointer
    {
        static_assert(BaseOf<Interface, DropConstVolatileReference<T>>);
    public:
        InterfacePointer(T* obj = nullptr) noexcept
            : obj(obj)
        {
            if(this->obj)
                this->obj->Interface::Acquire();
        }

        ~InterfacePointer()
        {
            if(this->obj)
                this->obj->Interface::Release();
        }

        InterfacePointer(const InterfacePointer& ifacep) noexcept
        {
            ifacep.obj->Interface::Acquire();
            this->obj = ifacep.obj;
        }

        InterfacePointer(InterfacePointer&& ifacep) noexcept
            : obj(Exchange(ifacep.obj, nullptr))
        {}

        InterfacePointer& operator=(const InterfacePointer& ifacep) noexcept
        {
            Reset();

            ifacep.obj->Interface::Acquire();
            this->obj = ifacep.obj;

            return *this;
        }

        InterfacePointer& operator=(InterfacePointer&& ifacep) noexcept
        {
            Reset();

            this->obj = Exchange(ifacep.obj, nullptr);

            return *this;
        }

        template<typename U>
        requires Constructible<T*, U*>
        InterfacePointer(const InterfacePointer<U>& ifacep) noexcept
        {
            ifacep.obj->Interface::Acquire();
            this->obj = ifacep.obj;
        }

        template<typename U>
        requires Constructible<T*, U*>
        InterfacePointer(InterfacePointer<U>&& ifacep) noexcept
            : obj(Exchange(ifacep.obj, nullptr))
        {}

        template<typename U>
        requires Constructible<T*, U*>
        InterfacePointer& operator=(const InterfacePointer<U>& ifacep) noexcept
        {
            Reset();

            ifacep.obj->Interface::Acquire();
            this->obj = ifacep.obj;

            return *this;
        }

        template<typename U>
        requires Constructible<T*, U*>
        InterfacePointer& operator=(InterfacePointer<U>&& ifacep) noexcept
        {
            Reset();

            this->obj = Exchange(ifacep.obj, nullptr);

            return *this;
        }

        void Reset() noexcept
        {
            if(this->obj)
            {
                this->obj->Interface::Release();
                this->obj = nullptr;
            }
        }

        T* operator->() const noexcept
        {
            return this->obj;
        }

        explicit operator bool() const noexcept
        {
            return this->obj != nullptr;
        }

        //both To and From can be const or not
        template<typename To>
        requires BaseOf<Interface, DropConstVolatileReference<To>> && SameAs<DropConstVolatileReference<To>, To> && Constructible<To*, T*>
        InterfacePointer<To> InterfaceCast() const noexcept
        {
            if(!this->obj)
                return InterfacePointer<To>{};

            return InterfacePointer<To>(::Core::InterfaceCast<To>(this->obj));
        }
    private:
        T* obj;
    };
};