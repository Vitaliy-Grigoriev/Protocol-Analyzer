// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <ratio>

#include "../../include/framework/Timer.hpp"

namespace analyzer::framework::diagnostic
{
    using namespace common;
    using nano_t   = std::chrono::duration<std::size_t, std::nano>;
    using micro_t  = std::chrono::duration<std::size_t, std::micro>;
    using milli_t  = std::chrono::duration<std::size_t, std::milli>;
    using second_t = std::chrono::duration<double, std::ratio<1>>;
    using minute_t = std::chrono::duration<double, std::ratio<60>>;
    using hour_t   = std::chrono::duration<double, std::ratio<3600>>;


    Timer::Timer (const bool start) noexcept
    {
        if (start) {
            Start();
        }
    }

    void Timer::Start() noexcept
    {
        count.state = true;
        count.lastStartTime = Clock::get();
    }

    Timer& Timer::Pause() noexcept
    {
        if (count.state) {
            count.totalTime += Clock::get() - count.lastStartTime;
            count.state = false;
        }
        return *this;
    }

    Timer& Timer::Reset (const bool start) noexcept
    {
        count.totalTime = { };
        count.lastStartTime = { };
        if (start) {
            Start();
        }
        return *this;
    }

    Timer::TimerCount& Timer::PauseAndGetCount() noexcept
    {
        if (count.state) {
            count.totalTime += Clock::get() - count.lastStartTime;
            count.state = false;
        }
        return count;
    }

    const Timer::TimerCount& Timer::UpdateAndGetCount() noexcept
    {
        if (count.state) {
            count.totalTime += Clock::get() - count.lastStartTime;
            count.lastStartTime = Clock::get();
        }
        return count;
    }

    const Timer::TimerCount& Timer::GetCount() const noexcept {
        return count;
    }


    std::size_t Timer::TimerCount::TimeSinceEpoch() const noexcept {
        return static_cast<std::size_t>(totalTime.time_since_epoch().count());
    }

    std::size_t Timer::TimerCount::NanoSeconds() const noexcept {
        return GetTime<nano_t>();
    }

    std::size_t Timer::TimerCount::MicroSeconds() const noexcept {
        return GetTime<micro_t>();
    }

    std::size_t Timer::TimerCount::MilliSeconds() const noexcept {
        return GetTime<milli_t>();
    }

    double Timer::TimerCount::Seconds() const noexcept {
        return GetTime<second_t>();
    }

    double Timer::TimerCount::Minutes() const noexcept {
        return GetTime<minute_t>();
    }

    double Timer::TimerCount::Hours() const noexcept {
        return GetTime<hour_t>();
    }


    Timer::TimerCount::operator double() const noexcept {
        return Seconds();
    }

    Timer::TimerCount::operator size_t() const noexcept {
        return MicroSeconds();
    }


    Timer::TimerCount Timer::TimerCount::operator+ (const Timer::TimerCount& other) const noexcept
    {
        TimePoint currFullTime = totalTime, otherFullTime = other.totalTime;
        if (state) {
            currFullTime += Clock::get() - lastStartTime;
        }
        if (other.state) {
            otherFullTime += Clock::get() - other.lastStartTime;
        }

        TimerCount result = { };
        result.totalTime = currFullTime + otherFullTime.time_since_epoch();
        return result;
    }

    Timer::TimerCount Timer::TimerCount::operator- (const Timer::TimerCount& other) const noexcept
    {
        TimePoint currFullTime = totalTime, otherFullTime = other.totalTime;
        if (state) {
            currFullTime += Clock::get() - lastStartTime;
        }
        if (other.state) {
            otherFullTime += Clock::get() - other.lastStartTime;
        }

        TimerCount result = { };
        if (currFullTime >= otherFullTime) {
            result.totalTime = currFullTime - otherFullTime.time_since_epoch();
        }
        return result;
    }

    Timer::TimerCount& Timer::TimerCount::operator+= (const Timer::TimerCount& other) noexcept
    {
        totalTime += other.totalTime.time_since_epoch();
        if (other.state) {
            totalTime += Clock::get() - other.lastStartTime;
        }
        return *this;
    }

    Timer::TimerCount& Timer::TimerCount::operator-= (const Timer::TimerCount& other) noexcept
    {
        TimePoint otherFullTime = other.totalTime;
        if (other.state) {
            otherFullTime += Clock::get() - other.lastStartTime;
        }

        if (totalTime >= otherFullTime) {
            totalTime -= otherFullTime.time_since_epoch();
        } else {
            totalTime = { };
        }
        return *this;
    }

    bool Timer::TimerCount::operator== (const Timer::TimerCount& other) const noexcept
    {
        using std::chrono::duration_cast;

        TimePoint currFullTime = totalTime, otherFullTime = other.totalTime;
        if (state) {
            currFullTime += Clock::get() - lastStartTime;
        }
        if (other.state) {
            otherFullTime += Clock::get() - other.lastStartTime;
        }
        return (currFullTime == otherFullTime);
    }

    bool Timer::TimerCount::operator< (const Timer::TimerCount& other) const noexcept
    {
        TimePoint currFullTime = totalTime, otherFullTime = other.totalTime;
        if (state) {
            currFullTime += Clock::get() - lastStartTime;
        }
        if (other.state) {
            otherFullTime += Clock::get() - other.lastStartTime;
        }
        return (currFullTime < otherFullTime);
    }

    bool Timer::TimerCount::operator> (const Timer::TimerCount& other) const noexcept
    {
        TimePoint currFullTime = totalTime, otherFullTime = other.totalTime;
        if (state) {
            currFullTime += Clock::get() - lastStartTime;
        }
        if (other.state) {
            otherFullTime += Clock::get() - other.lastStartTime;
        }
        return (currFullTime > otherFullTime);
    }

}  // namespace diagnostic.
