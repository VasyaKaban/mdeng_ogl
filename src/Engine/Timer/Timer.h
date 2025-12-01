#pragma once

#include <chrono>

namespace Engine
{
    class Timer
    {
    public:
        Timer() = default;
        ~Timer() = default;
        Timer(const Timer&) = default;
        Timer(Timer&&) = default;
        Timer& operator=(const Timer&) = default;
        Timer& operator=(Timer&&) = default;

        using Duration = std::chrono::duration<float, std::milli>;

        static Duration DurationFromFPS(float fps) noexcept;

        void Begin() noexcept;
        void End() noexcept;
        Duration GetDuration() const noexcept;
        float GetFPS() const noexcept;
        float AdjustUpdateFactor(float reference_factor) const noexcept; //updates per second
        void CorrectOnExceeded(Duration max) noexcept;
    private:
        std::chrono::system_clock::time_point start;
        std::chrono::system_clock::time_point end;
        Duration duration;
    };
};