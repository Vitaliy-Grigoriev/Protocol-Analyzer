// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com


#include <thread>
#include <csignal>

#include "../include/analyzer/Task.hpp"


namespace analyzer::task
{
    TaskContext::~TaskContext(void) = default;


    void TaskManager::signal_handler (int32_t sig) noexcept
    {

    }

    void* TaskManager::ThreadsManager (void* input) noexcept
    {
        LOG_TRACE("TaskManager.ThreadsManager: Thread 'ThreadsManager' started...");
        const auto pointer = static_cast<TaskManager::ThreadPool*>(input);
        static sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGINT);
        sigaddset(&mask, SIGTSTP);
        sigaddset(&mask, SIGCONT);

        while (true)
        {
            try { std::lock_guard<std::mutex> lock(pointer->work_mutex); }
            catch (const std::system_error& err) {
                LOG_ERROR("TaskManager.ThreadsManager: When lock mutex - '", err.what(), "'.");
                continue;
            }

            int32_t ret = pthread_sigmask(SIG_BLOCK, &mask, nullptr);
            if (ret != 0)
            {
                LOG_ERROR("TaskManager.ThreadsManager: When block signals - ", GET_ERROR(ret));
                continue;
            }

            const auto currentTime = time::now();
            for (auto it = pointer->id_set.begin(); it != pointer->id_set.end(); ++it)
            {
                const auto context = it->second.second;
                const auto start = context->GetStartTime();
                const uint32_t status = context->GetStatus();
                if (status == TASK_STATE_FINISHED || status == TASK_STATE_SKIP)
                {  // If task finished or task skip.
                    it = pointer->id_set.erase(it);
                    LOG_INFO("TaskManager.ThreadsManager: Task '", context->GetWorkerName(), "' finished.");
                }
                else if (start + context->GetTimeOut() >= currentTime
                         || (status != TASK_STATE_ERROR && status != TASK_STATE_PENDING && status != TASK_STATE_TIMEOUT))
                {  // If task timeout expired.
                    pthread_kill(it->second.first, SIGINT);
                    context->SetStatus(TASK_STATE_TIMEOUT);
                    LOG_INFO("TaskManager.ThreadsManager: Task timeout has expired: '", context->GetWorkerName(), "'.");
                }
                else if (start + context->GetTimeOut() >= currentTime + std::chrono::minutes(2))
                {  // If anyone do not receive result.
                    it = pointer->id_set.erase(it);
                    LOG_INFO("TaskManager.ThreadsManager: Task '", context->GetWorkerName(), "' don't return result.");
                }
            }

            ret = pthread_sigmask(SIG_UNBLOCK, &mask, nullptr);
            if (ret != 0)
            {
                LOG_ERROR("TaskManager.ThreadsManager: When unblock signals - ", GET_ERROR(ret));
            }

            // Only in this place we can get the signals.
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }

    TaskManager::TaskManager(void) noexcept
    {
        sigset_t set = { };
        sigemptyset(&set);
        sigaddset(&set, SIGINT);   // Interrupt the task.
        sigaddset(&set, SIGTSTP);  // Stop the task.
        sigaddset(&set, SIGCONT);  // Continue the task.

        struct sigaction act = { };
        act.sa_handler = signal_handler;
        act.sa_mask = set;
        const int32_t sig_int = sigaction(SIGINT, &act, nullptr);
        const int32_t sig_stop = sigaction(SIGTSTP, &act, nullptr);
        const int32_t sig_cont = sigaction(SIGCONT, &act, nullptr);
        if (sig_int == 0 && sig_stop == 0 && sig_cont == 0)
        {
            const int32_t result = pthread_create(&manager_thread_id, nullptr, ThreadsManager, &pool);
            if (result != 0) {
                LOG_ERROR("TaskManager.TaskManager: In function 'pthread_create' - ", GET_ERROR(result));
            }
            return;
        }

        if (sig_int != 0)  { LOG_ERROR("TaskManager.TaskManager: In function 'sigaction' with SIGINT - ", GET_ERROR(sig_int)); }
        if (sig_stop != 0) { LOG_ERROR("TaskManager.TaskManager: In function 'sigaction' with SIGTSTP - ", GET_ERROR(sig_stop)); }
        if (sig_cont != 0) { LOG_ERROR("TaskManager.TaskManager: In function 'sigaction' with SIGCONT - ", GET_ERROR(sig_cont)); }
    }

    std::size_t TaskManager::AddTask (Worker* const worker, TaskContext* const context) noexcept
    {
        pthread_t thread_id;
        context->SetStatus(TASK_STATE_INIT);
        context->SetStartTime(std::chrono::system_clock::now());

        int32_t result = pthread_create(&thread_id, nullptr, *worker, static_cast<void*>(context));
        if (result != 0)
        {
            context->SetExitCode(result);
            context->SetStatus(TASK_STATE_ERROR);
            LOG_ERROR("TaskManager.AddTask: In function 'pthread_create' - ", GET_ERROR(result));
            return 0;
        }
        context->SetStatus(TASK_STATE_IN_PROGRESS);

        result = pthread_setname_np(thread_id, context->GetWorkerName().c_str());
        if (result != 0) {
            LOG_WARNING("TaskManager.AddTask: In function 'pthread_setname_np' - ", GET_ERROR(result));
        }

        LOG_INFO("TaskManager.AddTask: Task '", context->GetWorkerName(), "' started.");
        return pool.AddID(task_value_t(thread_id, context));
    }

    void TaskManager::SkipTask (const std::size_t fd) noexcept
    {
        try { std::lock_guard<std::mutex> lock(pool.work_mutex); }
        catch (const std::system_error& err) {
            LOG_ERROR("TaskManager.SkipTask: When lock mutex - '", err.what(), "'.");
            return;
        }

        const auto id = pool.FindID(fd);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
        if (&id != nullptr)  // We can get 'nullptr' value if descriptor not found.
        {
#pragma clang diagnostic pop
            id.second->SetStatus(TASK_STATE_SKIP);
            pthread_kill(id.first, SIGINT);
        }
    }

    void TaskManager::ChangeTimeOut (const std::size_t fd, std::chrono::seconds timeout) noexcept
    {
        const auto id = pool.FindID(fd);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
        if (&id != nullptr)  // We can get 'nullptr' value if descriptor not found.
        {
#pragma clang diagnostic pop
            id.second->SetTimeOut(timeout);
        }
    }

    bool TaskManager::Wait (const std::size_t fd) noexcept
    {
        const auto id = pool.FindID(fd);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wtautological-pointer-compare"
        if (&id != nullptr)  // We can get 'nullptr' value if descriptor not found.
        {
#pragma clang diagnostic pop
            const int32_t result = pthread_join(id.first, nullptr);
            if (result != 0) {
                LOG_ERROR("TaskManager.Wait: In function 'pthread_join' - ", GET_ERROR(result));
                return false;
            }
        }
        return true;
    }

    bool TaskManager::WaitAll(void) noexcept
    {
        try { std::lock_guard<std::mutex> lock(pool.work_mutex); }
        catch (const std::system_error& err) {
            LOG_ERROR("TaskManager.WaitAll: When lock mutex - '", err.what(), "'.");
            return false;
        }

        for (auto&& it : pool.id_set)
        {
            const int32_t result = pthread_join(it.first, nullptr);
            if (result != 0) {
                LOG_ERROR("TaskManager.WaitAll: In function 'pthread_join' - ", GET_ERROR(result));
            }
        }
        return true;
    }

    TaskManager::~TaskManager(void) noexcept
    {
        pthread_kill(manager_thread_id, SIGINT);
        const int32_t result = pthread_join(manager_thread_id, nullptr);
        if (result != 0) {
            LOG_ERROR("TaskManager.~TaskManager: In function 'pthread_join' - ", GET_ERROR(result));
        }
    }




    std::size_t TaskManager::ThreadPool::AddID (const task_value_t& value) noexcept
    {
        try { std::lock_guard<std::mutex> lock(work_mutex); }
        catch (const std::system_error& err) {
            LOG_ERROR("TaskManager.ThreadPool.AddID: When lock mutex - '", err.what(), "'.");
            return 0;
        }

        std::size_t id = 0;
        while (true)
        {
            id = common::GetRandomValue<std::size_t>();
            if (id_set.emplace(id, value).second == true) {
                break;
            }
        }
        return id;
    }

    bool TaskManager::ThreadPool::RemoveID (const std::size_t value) noexcept
    {
        try { std::lock_guard<std::mutex> lock(work_mutex); }
        catch (const std::system_error& err) {
            LOG_ERROR("TaskManager.ThreadPool.RemoveID: When lock mutex - '", err.what(), "'.");
            return false;
        }

        return (id_set.erase(value) != 0);
    }

    const TaskManager::task_value_t& TaskManager::ThreadPool::FindID (std::size_t identifier) noexcept
    {
        try { std::lock_guard<std::mutex> lock(work_mutex); }
        catch (const std::system_error& err) {
            LOG_ERROR("TaskManager.ThreadPool.FindID: When lock mutex - '", err.what(), "'.");
            return *null_task_value_t;
        }

        const auto id = id_set.find(identifier);
        return (id != id_set.end() ? id->second : *null_task_value_t);
    }

}  // namespace task.
