#pragma once

#include <cstdint>

namespace Core
{
    template<typename T>
    class ClassID;

    class ClassIDBase
    {
    public:
        using Type = std::uint64_t;
    private:
        template<typename T>
        friend class ClassID;

        static Type GenerateID() noexcept;
    };

    template<typename T>
    class ClassID
    {
    public:
        static inline const ClassIDBase::Type ID = ClassIDBase::GenerateID();
    };
};