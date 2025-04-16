#pragma once 

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <unordered_map>

#include "croncpp.h"
#include "thread_pool.h"


class ChronoixScheduler 
{
public:
    using Task = std::function<void()>;

    ChronoixScheduler(size_t thread_count = std::thread::hardware_concurrency()) : running(false), next_job_id(1), thread_pool(thread_count) {}

    ~ChronoixScheduler()
    {
        stop();
    }

    // task add
    int add_job(const std::string& cron_expr, Task task)
    {
        std::lock_guard<std::mutex> lock(mutex);

        cron::cronexpr expr = cron::make_cron(cron_expr);
        int job_id = next_job_id ++;
        jobs[job_id] = Job{job_id, expr, task, cron::cron_next(expr, std::chrono::system_clock::now()), false}; 
        return job_id; 
    }

    // task remove
    void remove_job(int job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (jobs.count(job_id))
        {
            jobs.erase(job_id); 
        }
    }

    // task stop
    void pause_job(int job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (jobs.count(job_id))
        {
            jobs[job_id].paused = true;
        }
    }

    // task resume
    void resume_job(int job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (jobs.count(job_id))
        {
            jobs[job_id].paused = false; 
        }
    }

    // scheduler start
    void start()
    {
        if (running) 
        {
            return ;
        }

        running = true;

        worker = std::thread([this](){
            while (running)
            {
                auto now = std::chrono::system_clock::now();
                std::chrono::system_clock::time_point next_wakeup = now + std::chrono::hours(24); 

                {
                    std::lock_guard<std::mutex> lock(mutex); 
                    
                    for (auto& [id, job] : jobs)
                    {
                        if (job.paused)
                        {
                            continue;
                        }

                        if (now >= job.next)
                        {
                            // std::thread([task = job.task](){
                            //     try
                            //     {
                            //         task();
                            //     }
                            //     catch (...) 
                            //     {
                            //         std::cerr << "任务执行出错" << std::endl;
                            //     }
                            // }).detach();

                            try
                            {
                                thread_pool.submit(job.task); 
                            }
                            catch(const std::exception& e)
                            {
                                std::cerr << "task submit failed: " << e.what() << '\n';
                            }
                            
                            job.next = cron::cron_next(job.expr, now); 
                        }

                        if (job.next < next_wakeup)
                        {
                            next_wakeup = job.next; 
                        }
                    }
                }
                
                std::this_thread::sleep_until(next_wakeup); 
            }
        }); 
    }

    // scheduler stop
    void stop() 
    {
        running = false;

        if (worker.joinable())
        {
            worker.join(); 
        }
    }

private:
    struct Job
    {
        int id;
        cron::cronexpr expr;
        Task task;
        std::chrono::system_clock::time_point next;
        bool paused;
    }; 

    std::unordered_map<int, Job> jobs;
    std::thread worker;
    std::atomic<bool> running;
    std::atomic<int> next_job_id;
    std::mutex mutex;

    ThreadPool thread_pool; 
}; 
