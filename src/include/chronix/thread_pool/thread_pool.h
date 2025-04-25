#pragma once

#include <atomic>
#include <condition_variable>
#include <functional>
#include <future>
#include <iostream>
#include <mutex>
#include <queue>
#include <thread>
#include <vector>

class ThreadPool
{
public:
    explicit ThreadPool(
        size_t thread_count = std::thread::hardware_concurrency())
        : stop_flag(false)
    {
        for (size_t i = 0; i != thread_count; i++)
        {
            workers.emplace_back([this]() {
                for (;;)
                {
                    Task task;

                    {
                        std::unique_lock<std::mutex> lock(queue_mutex);
                        condition.wait(lock, [this]() {
                            return stop_flag || !tasks.empty();
                        });

                        if (stop_flag && tasks.empty())
                        {
                            return;
                        }

                        task = std::move(tasks.front());
                        tasks.pop();
                    }

                    task();
                }
            });
        }
    }

    // delete copy constructor and assignment operator
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;

    // submit a task to the thread pool
    template <class F, class... Args>
    auto submit(F&& f, Args&&... args)
        -> std::future<typename std::invoke_result<F, Args...>::type>
    {
        using return_type = typename std::invoke_result<F, Args...>::type;

        auto task = std::make_shared<std::packaged_task<return_type()>>(
            std::bind(std::forward<F>(f), std::forward<Args>(args)...));

        std::future<return_type> resp = task->get_future();
        {
            std::lock_guard<std::mutex> lock(queue_mutex);

            if (stop_flag)
            {
                throw std::runtime_error("submit on stopped ThreadPool");
            }

            tasks.emplace([task]() { (*task)(); });
        }
        condition.notify_one();
        return resp;
    }

    ~ThreadPool()
    {
        {
            std::lock_guard<std::mutex> lock(queue_mutex);
            stop_flag = true;
        }

        condition.notify_all();

        for (std::thread& worker : workers)
        {
            if (worker.joinable())
            {
                worker.join();
            }
        }
    }

private:
    using Task = std::function<void()>;

    std::vector<std::thread> workers;
    std::queue<Task> tasks;

    std::mutex queue_mutex;
    std::condition_variable condition;
    std::atomic<bool> stop_flag;
};