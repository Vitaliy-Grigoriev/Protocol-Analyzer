// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================

#include <atomic>
#include <chrono>

namespace analyzer::framework::common
{
    /**
     * @brief Type of framework clock ticks.
     */
    using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;


    /**
     * @class Clock   Clock.hpp   "include/framework/common/Clock.hpp"
     * @brief This class implements framework clock wrapper.
     */
    class Clock
    {
    public:
        Clock() = default;
        Clock (Clock &&) = delete;
        Clock (const Clock &) = delete;
        Clock & operator= (Clock &&) = delete;
        Clock & operator= (const Clock &) = delete;

        /**
         * @brief Method that returns system time.
         *
         * @return Current system time.
         */
        static TimePoint get() noexcept;

        /**
         * @brief Setter for the clock lag.
         *
         * @param [in] inLag - new lag value for clock.
         */
        template <typename Duration>
        static void setLag (Duration inLag) noexcept;

        /**
         * @brief Setter for the clock advance.
         *
         * @param [in] inAdvance - new advance value for clock.
         */
        template <typename Duration>
        static void setAdvance (Duration inAdvance) noexcept;

    private:
        static std::atomic<std::chrono::milliseconds> lag;
        static std::atomic<std::chrono::milliseconds> advance;
    };


    // Setter for the clock lag.
    template <typename Duration>
    void Clock::setLag (const Duration inLag) noexcept
    {
        lag.store(std::chrono::duration_cast<std::chrono::milliseconds>(inLag));
    }

    // Setter for the clock advance.
    template <typename Duration>
    void Clock::setAdvance (const Duration inAdvance) noexcept
    {
        advance.store(std::chrono::duration_cast<std::chrono::milliseconds>(inAdvance));
    }

}  // namespace common.
