// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

#include "../include/analyzer/Mutex.hpp"


namespace analyzer::system
{
    [[nodiscard]]
    bool LocalMutex::Lock(void) noexcept
    {
        const int32_t result = pthread_mutex_lock(&mutex);
        isAlreadyLocked.store(true, std::memory_order_seq_cst);
        return (result == 0 || result == EDEADLK);
    }

    [[nodiscard]]
    bool LocalMutex::TryLock(void) noexcept
    {
        isAlreadyLocked.store(true, std::memory_order_seq_cst);
        return (pthread_mutex_trylock(&mutex) == 0);
    }

    [[nodiscard]]
    bool LocalMutex::Unlock(void) noexcept
    {
        return (pthread_mutex_unlock(&mutex) == 0);
    }

    LocalMutex::~LocalMutex(void) noexcept
    {
        pthread_mutex_unlock(&mutex);
        pthread_mutex_destroy(&mutex);
    }


}  // namespace system.
