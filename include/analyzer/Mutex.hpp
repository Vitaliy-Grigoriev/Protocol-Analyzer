#pragma once
#ifndef PROTOCOL_ANALYZER_MUTEX_HPP
#define PROTOCOL_ANALYZER_MUTEX_HPP

#include <atomic>
#include <chrono>
#include <cerrno>
#include <pthread.h>

// In System library MUST NOT use any another framework libraries because it is a core library.


namespace analyzer::system
{
    /**
     * @class LocalMutex Mutex.hpp "include/analyzer/Mutex.hpp"
     * @brief This class defined the interface for POSIX-oriented mutex for specific usages.
     */
    class LocalMutex
    {
    private:
        /**
         * @var pthread_mutex_t mutex;
         * @brief A POSIX mutex value for thread-safe program working.
         */
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        /**
         * @var std::atomic_bool isAlreadyLockable;
         * @brief Boolean flag that indicates that this mutex was locked.
         */
        std::atomic_bool isAlreadyLocked = false;

    public:
        /**
         * @fn LocalMutex::LocalMutex(void);
         * @brief Default constructor.
         */
        LocalMutex(void) = default;

        LocalMutex (LocalMutex &&) = delete;
        LocalMutex (const LocalMutex &) = delete;
        LocalMutex & operator= (LocalMutex &&) = delete;
        LocalMutex & operator= (const LocalMutex &) = delete;


        bool Lock(void) noexcept;

        bool TryLock(void) noexcept;

        bool Unlock(void) noexcept;

        template <typename Duration>
        bool TryLockUntil (const std::chrono::time_point<std::chrono::system_clock, Duration>& time) noexcept
        {
            const auto sec = std::chrono::time_point_cast<std::chrono::seconds>(time);
            const auto nanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(time - sec);
            const struct timespec ts =
            {
                static_cast<std::time_t>(sec.time_since_epoch().count()),
                nanosec.count()
            };

            const int32_t result = pthread_mutex_timedlock(&mutex, &ts);
            isAlreadyLocked.store(true, std::memory_order_seq_cst);
            return (result == 0 || result == EDEADLK);
        }

        template <typename Rep, typename Period>
        bool TryLockFor (const std::chrono::duration<Rep, Period>& time) noexcept
        {
            return TryLockUntil(std::chrono::system_clock::now() + time);
        }

        inline void ResetFlag(void) noexcept { isAlreadyLocked.store(false, std::memory_order_seq_cst); }
        inline bool IsAlreadyLocked(void) const noexcept { return isAlreadyLocked.load(std::memory_order_seq_cst); }

        ~LocalMutex(void) noexcept;
    };


    /**
     * @class LockGuard Mutex.hpp "include/analyzer/Mutex.hpp"
     * @brief This class defined the interface for lock mutex in constructor and automatically unlock in destructor.
     *
     * @note This class only for LocalMutex class as original the std::lock_guard class in STL.
     */
    class LockGuard
    {
    private:
        /**
         * @var LocalMutex & mutex;
         * @brief Stored reference of input LocalMutex class.
         */
        LocalMutex& mutex;

    public:
        LockGuard(void) = delete;
        LockGuard (LockGuard &&) = delete;
        LockGuard (const LockGuard &) = delete;
        LockGuard & operator= (LockGuard &&) = delete;
        LockGuard & operator= (const LockGuard &) = delete;

        /**
         * @fn explicit LockGuard::LockGuard (LocalMutex &) noexcept;
         * @brief Constructor of LockGuard class.
         * @param [in] input - The reference of LocalMutex class.
         */
        explicit LockGuard (LocalMutex& input) noexcept
                : mutex(input)
        {
            mutex.Lock();
        }

        /**
         * @fn LockGuard::~LockGuard(void) noexcept;
         * @brief Destructor of LockGuard class.
         */
        ~LockGuard(void) noexcept {
            mutex.Unlock();
        }
    };

}  // namespace system.


#endif  // PROTOCOL_ANALYZER_MUTEX_HPP
