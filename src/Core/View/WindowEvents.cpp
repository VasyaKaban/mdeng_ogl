#include "WindowEvents.h"
#include <chrono>

namespace Core
{
    std::uint64_t GetEventTimestamp() noexcept
    {
        using T = std::chrono::duration<std::uint64_t, std::milli>;

        return std::chrono::duration_cast<T>(std::chrono::system_clock::now().time_since_epoch())
            .count();
    }
};