#include "ClassID.hpp"

namespace Core
{
    static ClassID CLASS_BASE_ID_COUNTER = 0;

    ClassID ClassIdentityBase::GenerateID() noexcept
    {
        return CLASS_BASE_ID_COUNTER++;
    }
};