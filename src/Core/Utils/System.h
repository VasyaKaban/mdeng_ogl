#pragma once

#include "Impl/Win32System.h"
#include "Traits.hpp"

namespace Core
{
    template<UnsignedIntegral I>
    I GenerateUnsignedInteger()
    {
        UInt8 data[sizeof(I)];
        Core::System::GetRandomBytes(data, sizeof(I));

        I out = 0;
        for(DeviceSize i = 0; i < sizeof(I); i++)
            out |= static_cast<I>(data[i]) << (i * 8);

        return out;
    }
};