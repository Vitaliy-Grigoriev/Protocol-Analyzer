#include <ratio>
#include <chrono>
#include <fstream>
#include <cstdint>


#pragma once
#ifndef HTTP2_ANALYZER_TIMER_HPP
#define HTTP2_ANALYZER_TIMER_HPP


namespace analyzer {
    /**
     * @namespace diagnostic
     * @brief The namespace that contain definitions of diagnostic tools.
     */
    namespace diagnostic {
        /**
         * @class Timer timer.hpp "include/analyzer/timer.hpp"
         * @brief Class that check the performance.
         */
        class Timer
        {
            typedef std::chrono::duration<std::size_t, std::nano>   nano_t;
            typedef std::chrono::duration<std::size_t, std::micro>  micro_t;
            typedef std::chrono::duration<std::size_t, std::milli>  milli_t;
            typedef std::chrono::duration<double, std::ratio<1>>    second_t;
            typedef std::chrono::duration<double, std::ratio<60>>   minute_t;
            typedef std::chrono::duration<double, std::ratio<3600>> hour_t;
            /**
             * @typedef typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint_t;
             * @brief The type of the internal container for time ticks in the Timer class.
             */
            typedef std::chrono::time_point<std::chrono::high_resolution_clock> timepoint_t;

        public:
            /**
             * @class Count timer.hpp "include/analyzer/timer.hpp"
             * @brief Class that contain the internal values of Timer class.
             */
            class Count {
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
                 * @fn template <typename T> typename T::rep GetTime() const;
                 * @brief Function that returns the time in the corresponding form.
                 * @tparam [in] T - The object of the std::chrono::duration.
                 * @return Return the time value in the various form.
                 */
                template <typename T>
                typename T::rep GetTime(void) const
                {
                    using std::chrono::duration_cast;
                    if (state) {
                        return duration_cast<T>(GetCurrentTime() - lastStartTime + totalTime.time_since_epoch()).count();
                    }
                    return duration_cast<T>(totalTime.time_since_epoch()).count();
                }

            public:
                std::size_t NanoSeconds(void) const;
                std::size_t MicroSeconds(void) const;
                std::size_t MilliSeconds(void) const;
                double Seconds(void) const;
                double Minutes(void) const;
                double Hours(void) const;

                /**
                 * @fn friend std::ostream & operator<< (std::ostream& out, const Count& in);
                 * @brief An overloaded operator in the output stream.
                 * @param [out] out - The object of the 'std::ostream &' type.
                 * @param [in] in - The object of the 'const Count &' type.
                 * @return Return the value of time in seconds.
                 */
                friend std::ostream & operator<< (std::ostream& out, const Count& in) {
                    out << in.Seconds();
                    return out;
                }
                operator double(void) const;
                operator size_t(void) const;
            };

        private:
            /**
             * @var Count count;
             * @brief The object of the Count class.
             */
            Count count = { };
            /**
             * @fn static inline std::chrono::time_point GetCurrentTime(void) const;
             * @brief Function that returns current time in ticks.
             * @return The current time in process ticks.
             */
            static inline timepoint_t GetCurrentTime(void) {
                return std::chrono::high_resolution_clock::now();
            }

        public:
            /**
             * @fn explicit Timer (bool = false);
             * @brief Constructor of Timer diagnostic class.
             * @param [in] start - Flag that indicate start the timer when constructor call or not.
             */
            explicit Timer (bool /*start*/ = false);
            /**
             * @fn void Start(void);
             * @brief Function that starts the timer.
             */
            void Start(void);
            /**
             * @fn Timer & Pause(void);
             * @brief Function that pauses the timer.
             * @return Timer - A reference to the Timer class.
             */
            Timer & Pause(void);
            /**
             * @fn Timer::Count & PauseGetCount(void);
             * @brief Function that pauses the timer and return a process ticks.
             * @return Timer::Count - A reference of the Count class.
             */
            Timer::Count & PauseGetCount(void);
            /**
             * @fn void Reset (bool = false);
             * @brief Function that resets the timer and starts it if needed.
             * @param [in] start - Flag that indicate start the timer when constructor call or not.
             */
            void Reset (bool /*start*/ = false);
            /**
             * @fn const Timer::Count & GetCount(void) const;
             * @brief Function that returns the reference of the Count class.
             * @return Timer::Count - A reference of the Count class.
             */
            const Timer::Count & GetCount(void) const;
            /**
             * @fn ~Timer(void) = default;
             * @brief Destructor of Timer diagnostic class.
             */
            ~Timer(void) = default;
        };

    }  // namespace diagnostic.
}  // namespace analyzer.

#endif  // HTTP2_ANALYZER_TIMER_HPP
