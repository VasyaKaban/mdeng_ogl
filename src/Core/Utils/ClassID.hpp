#pragma once

#include <cstdint>

namespace Core
{
    template<typename T>
    class ClassID;

    class ClassIDBase
    {
    private:
        template<typename T>
        friend class ClassID;

        static std::uint64_t GenerateID() noexcept;
    };

    template<typename T>
    class ClassID
    {
    public:
        static inline const std::uint64_t ID = ClassIDBase::GenerateID();
    };
};