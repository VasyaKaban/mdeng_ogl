#pragma once

#include <chrono>

class Timer
{
public:
    Timer() = default;
    ~Timer() = default;
    Timer(const Timer&) = default;
    Timer(Timer&&) = default;
    Timer& operator=(const Timer&) = default;
    Timer& operator=(Timer&&) = default;

    void Begin() noexcept;
    void End() noexcept;
    std::chrono::duration<float, std::milli> GetDuration() const noexcept;
    float GetFPS() const noexcept;
    float AdjustUpdateFactor(float reference_factor) const noexcept; //updates per millisecond
private:
    std::chrono::system_clock::time_point start;
    std::chrono::system_clock::time_point end;
    std::chrono::duration<float, std::milli> duration;
};