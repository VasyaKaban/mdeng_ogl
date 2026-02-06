#pragma once

#include <cstdint>
#include "Core/API.h"

namespace Core
{
    template<typename T>
    class ClassID;

    class CORE_API ClassIDBase
    {
    public:
        using ClassIDType = std::uint64_t;
    private:
        template<typename T>
        friend class ClassID;

        static ClassIDType GenerateID() noexcept;
    };

    template<typename T>
    class ClassID
    {
    public:
        static inline const ClassIDBase::ClassIDType ID = ClassIDBase::GenerateID();
    };
};