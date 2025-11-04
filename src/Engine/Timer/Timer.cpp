#include "Timer.h"
#include <thread>

Timer::Duration Timer::DurationFromFPS(float fps) noexcept
{
    //fps - frames per second -> 60 fps = 16.7ms
    //dur = 1000ms / fps
    return Duration(1000) / fps;
}

void Timer::Begin() noexcept
{
    start = std::chrono::system_clock::now();
}

void Timer::End() noexcept
{
    end = std::chrono::system_clock::now();
    duration = end - start;
}

Timer::Duration Timer::GetDuration() const noexcept
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

void Timer::CorrectOnExceeded(Duration max) noexcept
{
    if(duration < max)
    {
        //sleep-like
        auto dur_to_sleep = max - duration;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(static_cast<std::uint64_t>(dur_to_sleep.count())));

        End();
    }
}