#include "ClassID.hpp"

namespace Core
{
    static std::uint64_t CLASS_BASE_ID_COUNTER = 0;

    std::uint64_t ClassIDBase::GenerateID() noexcept
    {
        return CLASS_BASE_ID_COUNTER++;
    }
};