#pragma once

#include "UUID.hpp"
#include "Traits.hpp"
#include "Core/API.h"

namespace Core
{
#define CORE_INTERFACE_ID(VALUE) constexpr static UUID InterfaceIdentity = u8##VALUE;
#define CORE_INTERFACE_GET_ID(CLASS, ...) CLASS __VA_OPT__(, __VA_ARGS__)::InterfaceIdentity

    class CORE_API Interface
    {
    public:
        CORE_INTERFACE_ID("57e7739d-9466-4a43-8e02-c0ef30a14df9")

        virtual ~Interface() = 0;

        //Cast: check that current object implements(has in hierarchy class with current ClassID) and performs inner cast to the desired class type(with possible class disambiguation)
        //This methods should be implemented for interfaces and for classes(in case of ambiguation)
        virtual Void* Cast(const UUID& id) noexcept;
    };

    //both To and From can be const or not
    template<typename To, typename From>
    requires BaseOf<Interface, DropConstVolatileReference<To>> && SameAs<DropConstVolatileReference<To>, To> && BaseOf<Interface, DropConstVolatileReference<From>> &&
             SameAs<DropConstVolatileReference<From>, From>
    To* InterfaceCast(From* from) noexcept
    {
        const auto& id = CORE_INTERFACE_GET_ID(DropConstVolatileReference<To>);

        return static_cast<To*>(from->Cast(id));
    }

};