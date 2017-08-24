#pragma once
#ifndef HTTP2_ANALYZER_TASK_HPP
#define HTTP2_ANALYZER_TASK_HPP


#include <mutex>
#include <atomic>
#include <vector>
#include <pthread.h>
#include <unordered_map>

#include "Common.hpp"


namespace analyzer::task
{
    using time = std::chrono::system_clock;

    /**
     * @enum TASK_STATES
     * @brief The task statuses.
     */
    enum TASK_STATES : uint32_t
    {
        TASK_STATE_ERROR = 0,        // This state set if the task finished with error and wait for return the result.
        TASK_STATE_TIMEOUT = 1,      // This state set if timeout expired (after that the id live 5 seconds).
        TASK_STATE_SKIP = 2,         // This state set if anyone skip the task.
        TASK_STATE_INIT = 3,         // This state set in constructor.
        TASK_STATE_IN_PROGRESS = 4,  // This state set in start function before start new thread with task.
        TASK_STATE_PENDING = 5,      // This state set in thread after the task finished and wait for return the result.
        TASK_STATE_FINISHED = 6      // This state set in wait functions after the result was returned.
    };

    /**
      * @class Context Task.hpp "include/analyzer/Task.hpp"
      * @brief The context of the new task that determines its status and needed values.
      * @note The worker thread must be lock the mutex 'work_t' on the start and unlock it on the finish.
      */
    class Context
    {
    private:
        const std::string workerName;
        mutable time::time_point start_time;
        std::atomic<std::chrono::seconds> timeout;
        std::atomic<uint32_t> status;
        std::atomic<int32_t> exitCode;

    public:
        Context (Context &&) = delete;
        Context (const Context &) = delete;
        Context & operator= (Context &&) = delete;
        Context & operator= (const Context &) = delete;


        explicit Context (const std::string& name, uint32_t time_out = DEFAULT_TIMEOUT_TASK_THREAD) noexcept
                : workerName(name), timeout(std::chrono::seconds(time_out)), status(TASK_STATE_INIT), exitCode(0)
        { }

        inline std::string GetWorkerName(void) const noexcept { return workerName; }

        inline void SetStartTime(time::time_point start) noexcept { start_time = start; }
        inline time::time_point GetStartTime(void) const noexcept { return start_time; }

        inline void SetTimeOut (std::chrono::seconds new_timeout) noexcept { timeout.store(new_timeout, std::memory_order_release); }
        inline std::chrono::seconds GetTimeOut(void) const noexcept { return timeout.load(std::memory_order_acquire); }

        inline void SetStatus (const uint32_t new_status) noexcept { status.store(new_status, std::memory_order_seq_cst); }
        inline uint32_t GetStatus(void) const noexcept { return status.load(std::memory_order_seq_cst); }

        inline void SetExitCode (const int32_t new_code) noexcept { exitCode.store(new_code, std::memory_order_relaxed); }
        inline int32_t GetExitCode(void) const noexcept { return exitCode.load(std::memory_order_relaxed); }

        virtual ~Context(void);
    };

    /**
      * @typedef typedef void * (*Worker)(void *);
      * @brief The type of the internal thread worker in TaskManager.
      * @note The Worker function starts in new thread.
      */
    using Worker = void * (*) (void *);


    /**
     * @class TaskManager Task.hpp "include/analyzer/Task.hpp"
     * @brief This class defined the work with pool of tasks.
     * @note This class is not thread safe.
     */
    class TaskManager
    {
        using task_value_t = std::pair<pthread_t, Context *>;

    private:
        /**
         * @class PoolID Task.hpp "include/analyzer/Task.hpp"
         * @brief This class defined the work with pool of thread IDs.
         * @note It is the internal class of TaskManager class.
         */
        class PoolID
        {
            friend class TaskManager;

        private:
            std::mutex work_mutex = { };
            const task_value_t * const null_task_value_t = nullptr;
            std::unordered_map<std::size_t, task_value_t> id_set = { };

        public:
            PoolID(void) = default;
            ~PoolID(void) = default;

            PoolID (PoolID &&) = delete;
            PoolID (const PoolID &) = delete;
            PoolID & operator= (PoolID &&) = delete;
            PoolID & operator= (const PoolID &) = delete;


            std::size_t AddID (const task_value_t & /*value*/) noexcept;
            bool RemoveID (std::size_t /*value*/) noexcept;
            const task_value_t & FindID (std::size_t /*identifier*/) noexcept;
        };

        static void signal_handler (int32_t /*sig*/) noexcept;
        static void * ThreadsManager (void * /*input*/) noexcept;

    private:
        pthread_t manager_thread_id;
        PoolID pool = { };

    public:
        TaskManager (TaskManager &&) = delete;
        TaskManager (const TaskManager &) = delete;
        TaskManager & operator= (TaskManager &&) = delete;
        TaskManager & operator= (const TaskManager &) = delete;

        // Constructor.
        TaskManager(void) noexcept;

        // Adding a new task to the pool.
        std::size_t AddTask (Worker * /*worker*/, Context * /*context*/) noexcept;
        // Skip the task.
        void SkipTask (std::size_t /*fd*/) noexcept;

        // Change timeout for certain thread work.
        void ChangeTimeOut (std::size_t /*fd*/, std::chrono::seconds /*timeout*/) noexcept;
        // Wait thread by descriptor (block operation).
        bool Wait (std::size_t /*fd*/) noexcept;
        // Wait all task threads (block operation).
        bool WaitAll(void) noexcept;

        // Destructor.
        ~TaskManager(void) noexcept;
    };

}  // namespace task.

#endif  // HTTP2_ANALYZER_TASK_HPP
