#pragma once

#include <cstdint>
#include "Core/API.h"

namespace Core
{
    using ClassID = std::uint64_t;

    template<typename T>
    class ClassIdentity;

    class CORE_API ClassIdentityBase
    {
    private:
        template<typename T>
        friend class ClassIdentity;

        static ClassID GenerateID() noexcept;
    };

    template<typename T>
    class ClassIdentity
    {
    public:
        static inline const ClassID ID = ClassIdentityBase::GenerateID();
    };

#define CORE_CLASS_ID(TEMPLATE_API, API, TYPE) TEMPLATE_API template class API ClassIdentity<TYPE>;
};