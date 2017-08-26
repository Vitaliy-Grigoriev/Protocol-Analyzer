// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include "../include/analyzer/Timer.hpp"


namespace analyzer::diagnostic
{
    Timer::Timer (bool start) noexcept
    {
        if (start == true) { Start(); }
    }

    void Timer::Start(void) noexcept
    {
        count.state = true;
        count.lastStartTime = GetCurrentTime();
    }

    Timer& Timer::Pause(void) noexcept
    {
        if (count.state == true) {
            count.totalTime += GetCurrentTime() - count.lastStartTime;
            count.state = false;
        }
        return *this;
    }

    Timer::TimeCount& Timer::PauseGetCount(void) noexcept
    {
        if (count.state == true) {
            count.totalTime += GetCurrentTime() - count.lastStartTime;
            count.state = false;
        }
        return count;
    }

    void Timer::Reset (bool start) noexcept
    {
        count.totalTime = { };
        count.lastStartTime = { };
        if (start == true) { Start(); }
    }

    const Timer::TimeCount& Timer::GetCount(void) const noexcept {
        return count;
    }



    std::size_t Timer::TimeCount::NanoSeconds(void) const noexcept {
        return GetTime<nano_t>();
    }

    std::size_t Timer::TimeCount::MicroSeconds(void) const noexcept {
        return GetTime<micro_t>();
    }

    std::size_t Timer::TimeCount::MilliSeconds(void) const noexcept {
        return GetTime<milli_t>();
    }

    double Timer::TimeCount::Seconds(void) const noexcept {
        return GetTime<second_t>();
    }

    double Timer::TimeCount::Minutes(void) const noexcept {
        return GetTime<minute_t>();
    }

    double Timer::TimeCount::Hours(void) const noexcept {
        return GetTime<hour_t>();
    }

    Timer::TimeCount:: operator double(void) const noexcept {
        return Seconds();
    }

    Timer::TimeCount::operator size_t(void) const noexcept {
        return MicroSeconds();
    }

}  // namespace diagnostic.





