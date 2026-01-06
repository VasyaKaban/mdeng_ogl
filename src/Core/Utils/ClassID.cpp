#include "ClassID.hpp"

namespace Core
{
    static ClassIDBase::Type CLASS_BASE_ID_COUNTER = 0;

    ClassIDBase::Type ClassIDBase::GenerateID() noexcept
    {
        return CLASS_BASE_ID_COUNTER++;
    }
};