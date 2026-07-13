#include "../Interface.h"

namespace Core
{
    Interface::~Interface()
    {}

    const Void* Interface::Cast(const UUID& id) const noexcept
    {
        if(id == CORE_INTERFACE_GET_ID(Interface))
            return this;

        return nullptr;
    }
};