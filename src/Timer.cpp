// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include "../include/analyzer/Timer.hpp"


namespace analyzer {
    namespace diagnostic {

        Timer::Timer (bool start) noexcept
        {
            if (start) { Start(); }
        }

        void Timer::Start(void) noexcept
        {
            count.state = true;
            count.lastStartTime = GetCurrentTime();
        }

        Timer& Timer::Pause(void) noexcept
        {
            if (count.state) {
                count.totalTime += GetCurrentTime() - count.lastStartTime;
                count.state = false;
            }
            return *this;
        }

        Timer::Count& Timer::PauseGetCount(void) noexcept
        {
            if (count.state) {
                count.totalTime += GetCurrentTime() - count.lastStartTime;
                count.state = false;
            }
            return count;
        }

        void Timer::Reset (bool start) noexcept
        {
            count.totalTime = { };
            count.lastStartTime = { };
            if (start) { Start(); }
        }

        const Timer::Count& Timer::GetCount(void) const noexcept {
            return count;
        }



        std::size_t Timer::Count::NanoSeconds(void) const noexcept {
            return GetTime<nano_t>();
        }

        std::size_t Timer::Count::MicroSeconds(void) const noexcept {
            return GetTime<micro_t>();
        }

        std::size_t Timer::Count::MilliSeconds(void) const noexcept {
            return GetTime<milli_t>();
        }

        double Timer::Count::Seconds(void) const noexcept {
            return GetTime<second_t>();
        }

        double Timer::Count::Minutes(void) const noexcept {
            return GetTime<minute_t>();
        }

        double Timer::Count::Hours(void) const noexcept {
            return GetTime<hour_t>();
        }

        Timer::Count:: operator double(void) const noexcept {
            return Seconds();
        }

        Timer::Count::operator size_t(void) const noexcept {
            return MicroSeconds();
        }

    }  // namespace diagnostic.
}  // namespace analyzer.





