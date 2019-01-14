// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com

// ============================================================================
// Copyright (c) 2017-2019, by Vitaly Grigoriev, <Vit.link420@gmail.com>.
// This file is part of ProtocolAnalyzer open source project under MIT License.
// ============================================================================


#include "../../include/framework/Mutex.hpp"


namespace analyzer::framework::system
{
    [[nodiscard]]
    bool LocalMutex::Lock(void) noexcept
    {
        isRequestForMutexLock.store(true, std::memory_order_seq_cst);
        const int32_t result = pthread_mutex_lock(&mutex);
        if (result == 0 || result == EDEADLK)
        {
            isAlreadyLocked.store(true, std::memory_order_seq_cst);
            return true;
        }
        return false;
    }

    [[nodiscard]]
    bool LocalMutex::TryLock(void) noexcept
    {
        isRequestForMutexLock.store(true, std::memory_order_seq_cst);
        if (pthread_mutex_trylock(&mutex) == 0)
        {
            isAlreadyLocked.store(true, std::memory_order_seq_cst);
            return true;
        }
        return false;
    }

    void LocalMutex::Unlock(void) noexcept
    {
        if (pthread_mutex_unlock(&mutex) == 0) {
            isAlreadyUnlocked.store(true, std::memory_order_seq_cst);
        }
    }

    LocalMutex::~LocalMutex(void) noexcept
    {
        pthread_mutex_unlock(&mutex);
        pthread_mutex_destroy(&mutex);
    }


}  // namespace system.
