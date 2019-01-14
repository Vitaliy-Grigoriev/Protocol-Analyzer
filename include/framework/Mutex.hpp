// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#ifndef PROTOCOL_ANALYZER_MUTEX_HPP
#define PROTOCOL_ANALYZER_MUTEX_HPP

#include <atomic>  // std::atomic_bool.
#include <chrono>  // system_clock::time_point, system_clock::now, duration, seconds, time_point_cast, duration_cast.
#include <cerrno>
#include <pthread.h>  // pthread_mutex_t.

// In System library MUST NOT use any another functional framework libraries because it is a core library.


namespace analyzer::framework::system
{
    /**
     * @class LocalMutex   Mutex.hpp   "include/framework/Mutex.hpp"
     * @brief This class defined the interface for exception-free POSIX-oriented mutex for specific usages.
     */
    class LocalMutex
    {
    private:
        /**
         * @brief A POSIX mutex value for thread-safe program working.
         */
        pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
        /**
         * @brief Boolean flag that indicates that there was a request for a mutex.
         */
        std::atomic_bool isRequestForMutexLock = false;
        /**
         * @brief Boolean flag that indicates that mutex was locked.
         */
        std::atomic_bool isAlreadyLocked = false;
        /**
         * @brief Boolean flag that indicates that mutex was unlocked.
         */
        std::atomic_bool isAlreadyUnlocked = false;

    public:
        /**
         * @brief Default constructor.
         */
        LocalMutex(void) = default;

        LocalMutex (LocalMutex &&) = delete;
        LocalMutex (const LocalMutex &) = delete;
        LocalMutex & operator= (LocalMutex &&) = delete;
        LocalMutex & operator= (const LocalMutex &) = delete;

        [[nodiscard]]
        bool Lock(void) noexcept;

        [[nodiscard]]
        bool TryLock(void) noexcept;

        void Unlock(void) noexcept;

        template <typename Duration>
        bool TryLockUntil (const std::chrono::time_point<std::chrono::system_clock, Duration>& time) noexcept
        {
            isRequestForMutexLock.store(true, std::memory_order_seq_cst);
            const auto sec = std::chrono::time_point_cast<std::chrono::seconds>(time);
            const auto nanosec = std::chrono::duration_cast<std::chrono::nanoseconds>(time - sec);
            const struct timespec ts =
            {
                static_cast<std::time_t>(sec.time_since_epoch().count()),
                nanosec.count()
            };

            const int32_t result = pthread_mutex_timedlock(&mutex, &ts);
            if (result == 0 || result == EDEADLK)
            {
                isAlreadyLocked.store(true, std::memory_order_seq_cst);
                return true;
            }
            return false;
        }

        template <typename Rep, typename Period>
        bool TryLockFor (const std::chrono::duration<Rep, Period>& time) noexcept
        {
            return TryLockUntil(std::chrono::system_clock::now() + time);
        }

        inline void ResetLockedFlag(void) noexcept { isAlreadyLocked.store(false, std::memory_order_seq_cst); }
        inline bool IsAlreadyLocked(void) const noexcept { return isAlreadyLocked.load(std::memory_order_seq_cst); }

        inline void ResetUnlockedFlag(void) noexcept { isAlreadyUnlocked.store(false, std::memory_order_seq_cst); }
        inline bool IsAlreadyUnlocked(void) const noexcept { return isAlreadyUnlocked.load(std::memory_order_seq_cst); }

        inline void ResetRequestFlag(void) noexcept { isRequestForMutexLock.store(false, std::memory_order_seq_cst); }
        inline bool IsRequestForLock(void) const noexcept { return isRequestForMutexLock.load(std::memory_order_seq_cst); }

        ~LocalMutex(void) noexcept;
    };


    /**
     * @class LockGuard   Mutex.hpp   "include/framework/Mutex.hpp"
     * @brief This class defined the interface for lock mutex in constructor and automatically unlock in destructor.
     *
     * @note This class only for LocalMutex class as original the std::lock_guard class in STL.
     */
    class LockGuard
    {
    private:
        /**
         * @brief Stored lvalue reference of LocalMutex class.
         */
        LocalMutex & mutex;

    public:
        LockGuard(void) = delete;
        LockGuard (LockGuard &&) = delete;
        LockGuard (const LockGuard &) = delete;
        LockGuard & operator= (LockGuard &&) = delete;
        LockGuard & operator= (const LockGuard &) = delete;

        /**
         * @brief Constructor of LockGuard class.
         *
         * @param [in] input - Lvalue reference of LocalMutex class.
         */
        explicit LockGuard (LocalMutex& input) noexcept
            : mutex(input)
        {
            [[maybe_unused]] bool unused = mutex.Lock();
        }

        /**
         * @brief Destructor of LockGuard class.
         */
        ~LockGuard(void) noexcept {
            mutex.Unlock();
        }
    };

}  // namespace system.


#endif  // PROTOCOL_ANALYZER_MUTEX_HPP
