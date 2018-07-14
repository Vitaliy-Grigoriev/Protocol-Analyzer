// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include "../../include/framework/Timer.hpp"


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
        if (count.state == true)
        {
            count.totalTime += GetCurrentTime() - count.lastStartTime;
            count.state = false;
        }
        return *this;
    }

    void Timer::Reset (bool start) noexcept
    {
        count.totalTime = { };
        count.lastStartTime = { };
        if (start == true) { Start(); }
    }

    Timer::TimerCount& Timer::PauseAndGetCount(void) noexcept
    {
        if (count.state == true)
        {
            count.totalTime += GetCurrentTime() - count.lastStartTime;
            count.state = false;
        }
        return count;
    }

    const Timer::TimerCount & Timer::UpdateAndGetCount(void) noexcept
    {
        if (count.state == true)
        {
            count.totalTime += GetCurrentTime() - count.lastStartTime;
            count.lastStartTime = GetCurrentTime();
        }
        return count;
    }

    const Timer::TimerCount& Timer::GetCount(void) const noexcept {
        return count;
    }


    std::size_t Timer::TimerCount::TimeSinceEpoch(void) const noexcept {
        return static_cast<std::size_t>(totalTime.time_since_epoch().count());
    }

    std::size_t Timer::TimerCount::NanoSeconds(void) const noexcept {
        return GetTime<nano_t>();
    }

    std::size_t Timer::TimerCount::MicroSeconds(void) const noexcept {
        return GetTime<micro_t>();
    }

    std::size_t Timer::TimerCount::MilliSeconds(void) const noexcept {
        return GetTime<milli_t>();
    }

    double Timer::TimerCount::Seconds(void) const noexcept {
        return GetTime<second_t>();
    }

    double Timer::TimerCount::Minutes(void) const noexcept {
        return GetTime<minute_t>();
    }

    double Timer::TimerCount::Hours(void) const noexcept {
        return GetTime<hour_t>();
    }


    Timer::TimerCount::operator double(void) const noexcept {
        return Seconds();
    }

    Timer::TimerCount::operator size_t(void) const noexcept {
        return MicroSeconds();
    }


    Timer::TimerCount Timer::TimerCount::operator+ (const Timer::TimerCount& other) const noexcept
    {
        timepoint_t currFullTime = totalTime, otherFullTime = other.totalTime;
        if (state == true) { currFullTime += GetCurrentTime() - lastStartTime; }
        if (other.state == true) { otherFullTime += GetCurrentTime() - other.lastStartTime; }

        TimerCount result = { };
        result.totalTime = currFullTime + otherFullTime.time_since_epoch();
        return result;
    }

    Timer::TimerCount Timer::TimerCount::operator- (const Timer::TimerCount& other) const noexcept
    {
        timepoint_t currFullTime = totalTime, otherFullTime = other.totalTime;
        if (state == true) { currFullTime += GetCurrentTime() - lastStartTime; }
        if (other.state == true) { otherFullTime += GetCurrentTime() - other.lastStartTime; }

        TimerCount result = { };
        if (currFullTime >= otherFullTime) {
            result.totalTime = currFullTime - otherFullTime.time_since_epoch();
        }
        return result;
    }

    Timer::TimerCount& Timer::TimerCount::operator+= (const Timer::TimerCount& other) noexcept
    {
        totalTime += other.totalTime.time_since_epoch();
        if (other.state == true) {
            totalTime += GetCurrentTime() - other.lastStartTime;
        }
        return *this;
    }

    Timer::TimerCount& Timer::TimerCount::operator-= (const Timer::TimerCount& other) noexcept
    {
        timepoint_t otherFullTime = other.totalTime;
        if (other.state == true) {
            otherFullTime += GetCurrentTime() - other.lastStartTime;
        }

        if (totalTime >= otherFullTime) {
            totalTime -= otherFullTime.time_since_epoch();
        }
        else { totalTime = { }; }
        return *this;
    }

    bool Timer::TimerCount::operator== (const Timer::TimerCount& other) const noexcept
    {
        using std::chrono::duration_cast;

        timepoint_t currFullTime = totalTime, otherFullTime = other.totalTime;
        if (state == true) { currFullTime += GetCurrentTime() - lastStartTime; }
        if (other.state == true) { otherFullTime += GetCurrentTime() - other.lastStartTime; }

        if (currFullTime > otherFullTime) {
            return (duration_cast<micro_t>(currFullTime.time_since_epoch()) -
                            duration_cast<micro_t>(otherFullTime.time_since_epoch()) <= micro_t(100));
        }
        return (duration_cast<micro_t>(otherFullTime.time_since_epoch()) -
                        duration_cast<micro_t>(currFullTime.time_since_epoch()) <= micro_t(100));
    }

    bool Timer::TimerCount::operator< (const Timer::TimerCount& other) const noexcept
    {
        timepoint_t currFullTime = totalTime, otherFullTime = other.totalTime;
        if (state == true) { currFullTime += GetCurrentTime() - lastStartTime; }
        if (other.state == true) { otherFullTime += GetCurrentTime() - other.lastStartTime; }
        return (currFullTime < otherFullTime);
    }

    bool Timer::TimerCount::operator> (const Timer::TimerCount& other) const noexcept
    {
        timepoint_t currFullTime = totalTime, otherFullTime = other.totalTime;
        if (state == true) { currFullTime += GetCurrentTime() - lastStartTime; }
        if (other.state == true) { otherFullTime += GetCurrentTime() - other.lastStartTime; }
        return (currFullTime > otherFullTime);
    }

}  // namespace diagnostic.
