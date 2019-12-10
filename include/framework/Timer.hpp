// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#pragma once

#include <ostream>
#include <cstddef>

#include "common/Clock.hpp"

namespace analyzer::framework::diagnostic
{
    /**
     * @class Timer   Timer.hpp   "include/framework/Timer.hpp"
     * @brief Class that checks the performance of code.
     */
    class Timer
    {
    public:
        /**
         * @class TimerCount   Timer.hpp   "include/framework/Timer.hpp"
         * @brief Class that contains the internal values of Timer class.
         *
         * @note It is the internal public class of Timer class.
         */
        class TimerCount
        {
            friend class Timer;

        private:
            /**
             * @brief Variable that indicates the overall time.
             */
            common::TimePoint totalTime;
            /**
             * @brief Variable that indicates the current time.
             */
            common::TimePoint lastStartTime;
            /**
             * @brief Flag that indicates the status of timer: work or not.
             */
            bool state = false;

            /**
             * @brief Method that returns the time in the corresponding form.
             *
             * @tparam [in] Type - The object of the std::chrono::duration.
             * @return Return time value in the corresponding form.
             */
            template <typename Type>
            typename Type::rep GetTime() const noexcept;

        public:
            /**
             * @brief Method that returns the amount of time between current and the clock's epoch.
             *
             * @return Time interval in raw count.
             */
            std::size_t TimeSinceEpoch() const noexcept;

            /**
             * @brief Method that returns time in nanoseconds.
             *
             * @return Time interval in nanoseconds.
             */
            std::size_t NanoSeconds() const noexcept;

            /**
             * @brief Method that returns time in microseconds.
             *
             * @return Time interval in microseconds.
             */
            std::size_t MicroSeconds() const noexcept;

            /**
             * @brief Method that returns time in milliseconds.
             *
             * @return Time interval in milliseconds.
             */
            std::size_t MilliSeconds() const noexcept;

            /**
             * @brief Method that returns time in seconds.
             *
             * @return Time interval in seconds.
             */
            double Seconds() const noexcept;

            /**
             * @brief Method that returns time in minutes.
             *
             * @return Time interval in minutes.
             */
            double Minutes() const noexcept;

            /**
             * @brief Method that returns time in hours.
             *
             * @return Time interval in hours.
             */
            double Hours() const noexcept;


            /**
             * @brief An overloaded operator in the output stream.
             *
             * @param [in,out] out - Reference of the 'std::ostream' type.
             * @param [in] in - Reference of the 'Timer::TimerCount' type.
             * @return Return time interval in seconds.
             */
            friend std::ostream& operator<< (std::ostream& out, const TimerCount& in) noexcept
            {
                out << in.Seconds();
                return out;
            }
            explicit operator double() const noexcept;
            explicit operator size_t() const noexcept;

            /**
             * @brief An overloaded member addition operator that modifies the internal time point.
             *
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return New object of the 'Timer::TimerCount' class.
             */
            TimerCount operator+ (const TimerCount & other) const noexcept;

            /**
             * @brief An overloaded member subtraction operator that modifies the internal time point.
             *
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return New object of the 'Timer::TimerCount' class.
             */
            TimerCount operator- (const TimerCount & other) const noexcept;

            /**
             * @brief An overloaded member addition assignment operator that modifies the internal time point.
             *
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return Reference of the current 'Timer::TimerCount' class.
             */
            TimerCount & operator+= (const TimerCount & other) noexcept;

            /**
             * @brief An overloaded member subtraction assignment operator that modifies the internal time point.
             *
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return Reference of the current 'Timer::TimerCount' class.
             */
            TimerCount & operator-= (const TimerCount & other) noexcept;

            /**
             * @brief An overloaded member equal compare operator.
             *
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return Boolean value that indicates about of compare result.
             *
             * @note Comparison occurs with predetermined accuracy of 100 microseconds.
             */
            bool operator== (const TimerCount & other) const noexcept;

            /**
             * @brief An overloaded member less compare operator.
             *
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return Boolean value that indicates about of compare result.
             */
            bool operator< (const TimerCount & other) const noexcept;

            /**
             * @brief An overloaded member greater compare operator.
             *
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return Boolean value that indicates about of compare result.
             */
            bool operator> (const TimerCount & other) const noexcept;

        };

    private:
        /**
         * @brief The object of the TimerCount class.
         */
        TimerCount count = { };

    public:
        Timer (Timer &&) = delete;
        Timer (const Timer &) = default;
        Timer & operator= (Timer &&) = delete;
        Timer & operator= (const Timer &) = delete;

        /**
         * @brief Constructor of Timer diagnostic class.
         *
         * @param [in] start - Flag that indicates status of the timer. Default: false.
         */
        explicit Timer (bool start = false) noexcept;

        /**
         * @brief Method that starts the timer.
         */
        void Start() noexcept;

        /**
         * @brief Method that pauses the timer and updates the total time in timer.
         *
         * @return Reference of the 'Timer' class.
         */
        Timer & Pause() noexcept;

        /**
         * @brief Method that resets the timer and starts it if needed.
         *
         * @param [in] start - Flag that indicates status of the timer. Default: false.
         *
         * @return Reference of the 'Timer' class.
         */
        Timer & Reset (bool start = false) noexcept;

        /**
         * @brief Method that pauses the timer, updates the total time in timer and return a process ticks.
         *
         * @return Reference of the 'Timer::TimerCount' class.
         */
        Timer::TimerCount & PauseAndGetCount() noexcept;

        /**
         * @brief Method that updates the total time in timer and return a process ticks.
         *
         * @return Reference of the 'Timer::TimerCount' class.
         */
        const Timer::TimerCount & UpdateAndGetCount() noexcept;

        /**
         * @brief Method that returns the reference of the 'Timer::TimerCount' class.
         *
         * @return Reference of the 'Timer::TimerCount' class.
         */
        const Timer::TimerCount & GetCount() const noexcept;
    };


    template <typename Type>
    typename Type::rep Timer::TimerCount::GetTime() const noexcept
    {
        using std::chrono::duration_cast;
        if (state) {
            return duration_cast<Type>(common::Clock::get() - lastStartTime + totalTime.time_since_epoch()).count();
        }
        return duration_cast<Type>(totalTime.time_since_epoch()).count();
    }

}  // namespace diagnostic.
