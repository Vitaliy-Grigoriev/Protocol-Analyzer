#include "common/Clock.hpp"

namespace analyzer::framework::common
{
    using namespace std::chrono;

    std::atomic<milliseconds> Clock::lag{0ms};
    std::atomic<milliseconds> Clock::advance{0ms};

    // Method that returns system time.
    TimePoint Clock::get() noexcept
    {
        return system_clock::now() - lag.load() + advance.load();
    }

}  // namespace common.
