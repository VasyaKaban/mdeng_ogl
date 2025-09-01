#include "Timer.h"

void Timer::Begin() noexcept
{
    start = std::chrono::system_clock::now();
}

void Timer::End() noexcept
{
    end = std::chrono::system_clock::now();
    duration = end - start;
}

std::chrono::duration<float, std::milli> Timer::GetDuration() const noexcept
{
    return duration;
}

float Timer::GetFPS() const noexcept
{
    return 1000.0f / duration.count();
}

float Timer::AdjustUpdateFactor(float reference_factor) const noexcept
{
    return reference_factor / GetFPS();
}