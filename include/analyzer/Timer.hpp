#pragma once
#ifndef PROTOCOL_ANALYZER_TIMER_HPP
#define PROTOCOL_ANALYZER_TIMER_HPP

#include <ratio>
#include <chrono>
#include <ostream>
#include <cstdint>


namespace analyzer::diagnostic
{
    /**
     * @class Timer Timer.hpp "include/analyzer/Timer.hpp"
     * @brief Class that check the performance.
     */
    class Timer
    {
    private:
        /**
         * @defgroup TIME_DURATIONS Time gaps.
         * @brief This durations defines the timer counts.
         *
         * @{
         */
        using nano_t   = std::chrono::duration<std::size_t, std::nano>;
        using micro_t  = std::chrono::duration<std::size_t, std::micro>;
        using milli_t  = std::chrono::duration<std::size_t, std::milli>;
        using second_t = std::chrono::duration<double, std::ratio<1>>;
        using minute_t = std::chrono::duration<double, std::ratio<60>>;
        using hour_t   = std::chrono::duration<double, std::ratio<3600>>;
        /**@}*/

        /**
         * @typedef typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint_t;
         * @brief The type of the internal container for time ticks in the Timer class.
         */
        using timepoint_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

    public:
        /**
         * @class TimerCount Timer.hpp "include/analyzer/Timer.hpp"
         * @brief Class that contain the internal values of Timer class.
         *
         * @note It is the internal class of Timer class.
         */
        class TimerCount
        {
            friend class Timer;

        private:
            /**
             * @var timepoint_t totalTime;
             * @brief Indicates the overall time of the timer.
             */
            timepoint_t totalTime = { };
            /**
             * @var timepoint_t lastStartTime;
             * @brief Indicates the current time of the timer.
             */
            timepoint_t lastStartTime = { };
            /**
             * @var bool state;
             * @brief Flag which indicates the status of timer: work or not.
             */
            bool state = false;

            /**
             * @fn template <typename T> typename T::rep GetTime(void) const noexcept;
             * @brief Function that returns the time in the corresponding form.
             * @tparam [in] T - The object of the std::chrono::duration.
             * @return Return the time value in the various form.
             */
            template <typename T>
            typename T::rep GetTime(void) const noexcept
            {
                using std::chrono::duration_cast;
                if (state == true) {
                    return duration_cast<T>(GetCurrentTime() - lastStartTime + totalTime.time_since_epoch()).count();
                }
                return duration_cast<T>(totalTime.time_since_epoch()).count();
            }

        public:
            /**
             * @fn std::size_t NanoSeconds(void) const noexcept;
             * @brief Function that returns time in nanoseconds.
             * @return Time interval in nanoseconds.
             */
            std::size_t NanoSeconds(void) const noexcept;

            /**
             * @fn std::size_t MicroSeconds(void) const noexcept;
             * @brief Function that returns time in microseconds.
             * @return Time interval in microseconds.
             */
            std::size_t MicroSeconds(void) const noexcept;

            /**
             * @fn std::size_t MilliSeconds(void) const noexcept;
             * @brief Function that returns time in milliseconds.
             * @return Time interval in milliseconds.
             */
            std::size_t MilliSeconds(void) const noexcept;

            /**
             * @fn double Seconds(void) const noexcept;
             * @brief Function that returns time in seconds.
             * @return Time interval in seconds.
             */
            double Seconds(void) const noexcept;

            /**
             * @fn double Minutes(void) const noexcept;
             * @brief Function that returns time in minutes.
             * @return Time interval in minutes.
             */
            double Minutes(void) const noexcept;

            /**
             * @fn double Hours(void) const noexcept;
             * @brief Function that returns time in hours.
             * @return Time interval in hours.
             */
            double Hours(void) const noexcept;

            /**
             * @fn friend std::ostream & operator<< (std::ostream &, const Count &) noexcept;
             * @brief An overloaded operator in the output stream.
             * @param [in,out] out - The object of the 'std::ostream &' type.
             * @param [in] in - The object of the 'const TimerCount &' type.
             * @return Return time interval in seconds.
             */
            friend std::ostream & operator<< (std::ostream& out, const TimerCount& in) noexcept
            {
                out << in.Seconds();
                return out;
            }
            explicit operator double(void) const noexcept;
            explicit operator size_t(void) const noexcept;
        };

    private:
        /**
         * @var TimerCount count;
         * @brief The object of the TimerCount class.
         */
        TimerCount count = { };

        /**
         * @fn static inline std::chrono::time_point GetCurrentTime(void) noexcept;
         * @brief Function that returns current time in ticks.
         * @return The current time in process ticks.
         */
        static inline timepoint_t GetCurrentTime(void) noexcept {
            return std::chrono::high_resolution_clock::now();
        }

    public:
        Timer (Timer &&) = delete;
        Timer (const Timer &) = default;
        Timer & operator= (Timer &&) = delete;
        Timer & operator= (const Timer &) = delete;

        /**
         * @fn explicit Timer (bool) noexcept;
         * @brief Constructor of Timer diagnostic class.
         * @param [in] start - Flag that indicate status of the timer. Default: false.
         */
        explicit Timer (bool /*start*/ = false) noexcept;

        /**
         * @fn void Start(void);
         * @brief Function that starts the timer.
         */
        void Start(void) noexcept;

        /**
         * @fn Timer & Pause(void) noexcept;
         * @brief Function that pauses the timer.
         * @return Timer - A reference to the Timer class.
         */
        Timer & Pause(void) noexcept;

        /**
         * @fn Timer::TimerCount & PauseGetCount(void) noexcept;
         * @brief Function that pauses the timer and return a process ticks.
         * @return Timer::TimerCount - A reference of the TimerCount class.
         */
        Timer::TimerCount & PauseGetCount(void) noexcept;

        /**
         * @fn void Reset (bool) noexcept;
         * @brief Function that resets the timer and starts it if needed.
         * @param [in] start - Flag that indicate status of the timer. Default: false.
         */
        void Reset (bool /*start*/ = false) noexcept;

        /**
         * @fn const Timer::TimerCount & GetCount(void) const noexcept;
         * @brief Function that returns the reference of the Count class.
         * @return Timer::TimerCount - A reference of the TimerCount class.
         */
        const Timer::TimerCount & GetCount(void) const noexcept;

        /**
         * @fn ~Timer(void) = default;
         * @brief Default destructor of Timer diagnostic class.
         */
        ~Timer(void) = default;
    };

}  // namespace diagnostic.

#endif  // PROTOCOL_ANALYZER_TIMER_HPP
