#ifndef CRT_THREAD_POOL_HPP
#define CRT_THREAD_POOL_HPP

#pragma once

#include <condition_variable>
#include <exception>
#include <functional>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class CRT_thread_pool
{
public:
    explicit CRT_thread_pool(size_t thread_count = std::thread::hardware_concurrency())
    {
        if (thread_count == 0) thread_count = 1; // hardware_concurrency() may return 0

        workers.reserve(thread_count);
        for (size_t i = 0; i < thread_count; ++i)
            workers.emplace_back([this] { worker_loop(); });
    }

    ~CRT_thread_pool()
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            stopping = true;
        }
        task_available.notify_all();
        for (std::thread& t : workers) t.join();
    }

    CRT_thread_pool(const CRT_thread_pool&)            = delete;
    CRT_thread_pool& operator=(const CRT_thread_pool&) = delete;

    void enqueue(std::function<void()> task)
    {
        {
            std::lock_guard<std::mutex> lock(mutex);
            tasks.push(std::move(task));
            ++pending;
        }
        task_available.notify_one();
    }

    // blocks until every enqueued task has finished; rethrows the first task exception
    void wait()
    {
        std::unique_lock<std::mutex> lock(mutex);
        all_done.wait(lock, [this] { return pending == 0; });

        if (first_error)
        {
            std::exception_ptr e = first_error;
            first_error = nullptr;
            std::rethrow_exception(e);
        }
    }

    size_t size() const { return workers.size(); }

private:
    void worker_loop()
    {
        for (;;)
        {
            std::function<void()> task;
            {
                std::unique_lock<std::mutex> lock(mutex);
                task_available.wait(lock, [this] { return stopping || !tasks.empty(); });

                if (stopping && tasks.empty()) return;

                task = std::move(tasks.front());
                tasks.pop();
            }

            try { task(); }
            catch (...)
            {
                std::lock_guard<std::mutex> lock(mutex);
                if (!first_error) first_error = std::current_exception();
            }

            {
                std::lock_guard<std::mutex> lock(mutex);
                if (--pending == 0) all_done.notify_all();
            }
        }
    }

    std::vector<std::thread>          workers;
    std::queue<std::function<void()>> tasks;
    std::mutex                        mutex;
    std::condition_variable           task_available;
    std::condition_variable           all_done;
    size_t                            pending = 0;
    bool                              stopping = false;
    std::exception_ptr                first_error;
};

#endif