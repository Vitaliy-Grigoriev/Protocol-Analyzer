// ============================================================================
// Copyright (c) 2017-2018, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#ifndef PROTOCOL_ANALYZER_TIMER_HPP
#define PROTOCOL_ANALYZER_TIMER_HPP

#include <ratio>
#include <chrono>
#include <ostream>
#include <cstddef>


namespace analyzer::framework::diagnostic
{
    /**
     * @class Timer Timer.hpp "include/framework/Timer.hpp"
     * @brief Class that checks the performance of code.
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
         * @brief Type of the internal container for time ticks in the Timer class.
         */
        using timepoint_t = std::chrono::time_point<std::chrono::high_resolution_clock>;

    public:
        /**
         * @class TimerCount Timer.hpp "include/framework/Timer.hpp"
         * @brief Class that contains the internal values of Timer class.
         *
         * @note It is the internal public class of Timer class.
         */
        class TimerCount
        {
            friend class Timer;

        private:
            /**
             * @var timepoint_t totalTime;
             * @brief Variable that indicates the overall time.
             */
            timepoint_t totalTime = { };
            /**
             * @var timepoint_t lastStartTime;
             * @brief Variable that indicates the current time.
             */
            timepoint_t lastStartTime = { };
            /**
             * @var bool state;
             * @brief Flag that indicates the status of timer: work or not.
             */
            bool state = false;

            /**
             * @fn template <typename Type>
             * typename Type::rep Timer::TimerCount::GetTime(void) const noexcept;
             * @brief Method that returns the time in the corresponding form.
             * @tparam [in] Type - The object of the std::chrono::duration.
             * @return Return time value in the corresponding form.
             */
            template <typename Type>
            typename Type::rep GetTime(void) const noexcept
            {
                using std::chrono::duration_cast;
                if (state == true) {
                    return duration_cast<Type>(GetCurrentTime() - lastStartTime + totalTime.time_since_epoch()).count();
                }
                return duration_cast<Type>(totalTime.time_since_epoch()).count();
            }

        public:
            /**
             * @fn std::size_t Timer::TimerCount::TimeSinceEpoch(void) const noexcept;
             * @brief Method that returns the amount of time between current and the clock's epoch.
             * @return Time interval in raw count.
             */
            std::size_t TimeSinceEpoch(void) const noexcept;

            /**
             * @fn std::size_t Timer::TimerCount::NanoSeconds(void) const noexcept;
             * @brief Method that returns time in nanoseconds.
             * @return Time interval in nanoseconds.
             */
            std::size_t NanoSeconds(void) const noexcept;

            /**
             * @fn std::size_t Timer::TimerCount::MicroSeconds(void) const noexcept;
             * @brief Method that returns time in microseconds.
             * @return Time interval in microseconds.
             */
            std::size_t MicroSeconds(void) const noexcept;

            /**
             * @fn std::size_t Timer::TimerCount::MilliSeconds(void) const noexcept;
             * @brief Method that returns time in milliseconds.
             * @return Time interval in milliseconds.
             */
            std::size_t MilliSeconds(void) const noexcept;

            /**
             * @fn double Timer::TimerCount::Seconds(void) const noexcept;
             * @brief Method that returns time in seconds.
             * @return Time interval in seconds.
             */
            double Seconds(void) const noexcept;

            /**
             * @fn double Timer::TimerCount::Minutes(void) const noexcept;
             * @brief Method that returns time in minutes.
             * @return Time interval in minutes.
             */
            double Minutes(void) const noexcept;

            /**
             * @fn double Timer::TimerCount::Hours(void) const noexcept;
             * @brief Method that returns time in hours.
             * @return Time interval in hours.
             */
            double Hours(void) const noexcept;


            /**
             * @fn friend std::ostream & operator<< (std::ostream &, const Timer::TimerCount &) noexcept;
             * @brief An overloaded operator in the output stream.
             * @param [in,out] out - Reference of the 'std::ostream' type.
             * @param [in] in - Reference of the 'Timer::TimerCount' type.
             * @return Return time interval in seconds.
             */
            friend std::ostream& operator<< (std::ostream& out, const TimerCount& in) noexcept
            {
                out << in.Seconds();
                return out;
            }
            explicit operator double(void) const noexcept;
            explicit operator size_t(void) const noexcept;

            /**
             * @fn Timer::TimerCount Timer::TimerCount::operator+ (const Timer::TimerCount &) const noexcept;
             * @brief An overloaded member addition operator that modifies the internal time point.
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return New object of the 'Timer::TimerCount' class.
             */
            TimerCount operator+ (const TimerCount & /*other*/) const noexcept;

            /**
             * @fn Timer::TimerCount Timer::TimerCount::operator- (const Timer::TimerCount &) const noexcept;
             * @brief An overloaded member subtraction operator that modifies the internal time point.
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return New object of the 'Timer::TimerCount' class.
             */
            TimerCount operator- (const TimerCount & /*other*/) const noexcept;

            /**
             * @fn Timer::TimerCount & Timer::TimerCount::operator+= (const Timer::TimerCount &) noexcept;
             * @brief An overloaded member addition assignment operator that modifies the internal time point.
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return Reference of the current 'Timer::TimerCount' class.
             */
            TimerCount & operator+= (const TimerCount & /*other*/) noexcept;

            /**
             * @fn Timer::TimerCount & Timer::TimerCount::operator-= (const Timer::TimerCount &) noexcept;
             * @brief An overloaded member subtraction assignment operator that modifies the internal time point.
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return Reference of the current 'Timer::TimerCount' class.
             */
            TimerCount & operator-= (const TimerCount & /*other*/) noexcept;

            /**
             * @fn bool Timer::TimerCount::operator== (const Timer::TimerCount &) const noexcept;
             * @brief An overloaded member equal compare operator.
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return Boolean value that indicates about of compare result.
             *
             * @note Comparison occurs with predetermined accuracy of 100 microseconds.
             */
            bool operator== (const TimerCount & /*other*/) const noexcept;

            /**
             * @fn bool Timer::TimerCount::operator< (const Timer::TimerCount &) const noexcept;
             * @brief An overloaded member less compare operator.
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return Boolean value that indicates about of compare result.
             */
            bool operator< (const TimerCount & /*other*/) const noexcept;

            /**
             * @fn bool Timer::TimerCount::operator> (const Timer::TimerCount &) const noexcept;
             * @brief An overloaded member greater compare operator.
             * @param [in] other - Reference of the 'Timer::TimerCount' type.
             * @return Boolean value that indicates about of compare result.
             */
            bool operator> (const TimerCount & /*other*/) const noexcept;

        };

    private:
        /**
         * @var TimerCount count;
         * @brief The object of the TimerCount class.
         */
        TimerCount count = { };

        /**
         * @fn static inline std::chrono::time_point Timer::GetCurrentTime(void) noexcept;
         * @brief Method that returns current time in ticks.
         * @return Current time in process ticks.
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
         * @fn explicit Timer::Timer (bool) noexcept;
         * @brief Constructor of Timer diagnostic class.
         * @param [in] start - Flag that indicates status of the timer. Default: false.
         */
        explicit Timer (bool /*start*/ = false) noexcept;

        /**
         * @fn void Timer::Start(void);
         * @brief Method that starts the timer.
         */
        void Start(void) noexcept;

        /**
         * @fn Timer & Timer::Pause(void) noexcept;
         * @brief Method that pauses the timer and updates the total time in timer.
         * @return Timer - Reference of the 'Timer' class.
         */
        Timer & Pause(void) noexcept;

        /**
         * @fn void Timer::Reset (bool) noexcept;
         * @brief Method that resets the timer and starts it if needed.
         * @param [in] start - Flag that indicates status of the timer. Default: false.
         */
        void Reset (bool /*start*/ = false) noexcept;

        /**
         * @fn Timer::TimerCount & Timer::PauseAndGetCount(void) noexcept;
         * @brief Method that pauses the timer, updates the total time in timer and return a process ticks.
         * @return Timer::TimerCount - Reference of the 'Timer::TimerCount' class.
         */
        Timer::TimerCount & PauseAndGetCount(void) noexcept;

        /**
         * @fn const Timer::TimerCount & Timer::UpdateAndGetCount(void) noexcept;
         * @brief Method that updates the total time in timer and return a process ticks.
         * @return Timer::TimerCount - Reference of the 'Timer::TimerCount' class.
         */
        const Timer::TimerCount & UpdateAndGetCount(void) noexcept;

        /**
         * @fn const Timer::TimerCount & Timer::GetCount(void) const noexcept;
         * @brief Method that returns the reference of the 'Timer::TimerCount' class.
         * @return Timer::TimerCount - Reference of the 'Timer::TimerCount' class.
         */
        const Timer::TimerCount & GetCount(void) const noexcept;

        /**
         * @fn Timer::~Timer(void);
         * @brief Default destructor of the Timer diagnostic class.
         */
        ~Timer(void) = default;
    };

}  // namespace diagnostic.

#endif  // PROTOCOL_ANALYZER_TIMER_HPP
