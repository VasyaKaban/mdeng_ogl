#include "Interface.h"

namespace Core
{
    Interface::~Interface()
    {}

    bool Interface::Implements(ClassID id) const noexcept
    {
        return id == ClassIdentity<Interface>::ID;
    }
};