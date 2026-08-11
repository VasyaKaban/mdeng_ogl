#pragma once

namespace Core
{
    enum class MemoryOrder
    {
        Relaxed,
        Acquire,
        Release,
        AcquireRelease,
        SequentialConsistency
    };
};