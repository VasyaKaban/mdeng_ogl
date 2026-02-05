#pragma once

namespace hrs
{
    template<typename T>
    struct type_arg
    {
        using type = T;
    };

    template<typename T>
    using type_arg_t = type_arg<T>;
};