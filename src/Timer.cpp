// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include <string.h>
#include "../include/analyzer/Timer.hpp"


namespace analyzer {
    namespace diagnostic {

        Timer::Timer (bool start)
        {
            if (start) { Start(); }
        }

        void Timer::Start(void)
        {
            count.state = true;
            count.lastStartTime = GetCurrentTime();
        }

        Timer& Timer::Pause(void)
        {
            if (count.state) {
                count.totalTime += GetCurrentTime() - count.lastStartTime;
                count.state = false;
            }
            return *this;
        }

        Timer::Count& Timer::PauseGetCount(void)
        {
            if (count.state) {
                count.totalTime += GetCurrentTime() - count.lastStartTime;
                count.state = false;
            }
            return count;
        }

        void Timer::Reset (bool start)
        {
            count.totalTime = { };
            count.lastStartTime = { };
            if (start) { Start(); }
        }

        const Timer::Count& Timer::GetCount(void) const {
            return count;
        }



        std::size_t Timer::Count::NanoSeconds(void) const {
            return GetTime<nano_t>();
        }

        std::size_t Timer::Count::MicroSeconds(void) const {
            return GetTime<micro_t>();
        }

        std::size_t Timer::Count::MilliSeconds(void) const {
            return GetTime<milli_t>();
        }

        double Timer::Count::Seconds(void) const {
            return GetTime<second_t>();
        }

        double Timer::Count::Minutes() const {
            return GetTime<minute_t>();
        }

        double Timer::Count::Hours(void) const {
            return GetTime<hour_t>();
        }

        Timer::Count::operator double(void) const {
            return Seconds();
        }

        Timer::Count::operator size_t(void) const {
            return MicroSeconds();
        }

    }  // namespace diagnostic.
}  // namespace analyzer.





