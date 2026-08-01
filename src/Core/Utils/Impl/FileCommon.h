#pragma once

#include "../Types.hpp"

namespace Core
{
    using FileOpenFlags = UInt64;
    namespace FileOpenFlagBits
    {
        constexpr inline FileOpenFlags Read = 0b1 << 0;
        constexpr inline FileOpenFlags Write = 0b1 << 1;
        constexpr inline FileOpenFlags Append = 0b1 << 2;
        constexpr inline FileOpenFlags Create = 0b1 << 3; //create new file if it is not created with requested name(works only when either 'Write' or 'Append' is also included into mask)
    };

    enum class FilePointerStartingPoint : UInt32
    {
        Begin,
        End,
        Current
    };
};