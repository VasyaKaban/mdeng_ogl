#include "ClassID.hpp"

namespace Core
{
    static ClassIDBase::ClassIDType CLASS_BASE_ID_COUNTER = 0;

    ClassIDBase::ClassIDType ClassIDBase::GenerateID() noexcept
    {
        return CLASS_BASE_ID_COUNTER++;
    }
};