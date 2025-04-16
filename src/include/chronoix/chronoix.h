#pragma once 

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <unordered_map>

#include "chronoix/define.h" 

#include "chronoix/croncpp.h"
#include "chronoix/thread_pool/thread_pool.h"
#include "chronoix/persistence/persistence.h"


class ChronoixScheduler 
{
public:
    ChronoixScheduler(size_t thread_count = std::thread::hardware_concurrency()) : running(false), next_job_id(1), thread_pool(thread_count) {}

    ~ChronoixScheduler()
    {
        stop();
    }

    // task add
    int add_job(const std::string& cron_expr, Task task, ErrorCallback error_callback = nullptr, SuccessCallback success_callback = nullptr)
    {
        std::lock_guard<std::mutex> lock(mutex);

        cron::cronexpr expr = cron::make_cron(cron_expr);
        int job_id = next_job_id ++;
        jobs[job_id] = Job{job_id, expr, cron_expr, task, cron::cron_next(expr, std::chrono::system_clock::now()), false, error_callback, success_callback}; 
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

                            // try
                            // {
                            //     thread_pool.submit(job.task); 
                            //     if (job.success_callback)
                            //     {
                            //         job.success_callback(job.id); 
                            //     }
                            // }
                            // catch(const std::exception& e)
                            // {

                            //     std::cerr << "task submit failed: " << e.what() << '\n';
                            // }

                            auto wrapped_task = [this, job]() {
                                try 
                                {
                                    job.task(); 
                                    if (job.success_callback)
                                    {
                                        job.success_callback(job.id); 
                                    }
                                }
                                catch (const std::exception& e)
                                {
                                    if (job.error_callback)
                                    {
                                        job.error_callback(job.id, e); 
                                    }
                                }
                            }; 

                            thread_pool.submit(wrapped_task); 
                            
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

    // set persistence
    void set_persistence(std::shared_ptr<Persistence<Job>> persistence_backend)
    {
        persistence = persistence_backend; 
    }  

    void save_state()
    {
        if (persistence)
        {
            std::lock_guard<std::mutex> lock(mutex); 
            std::vector<Job> snapshot; 
            for (auto& [id, job] : jobs)
            {
                snapshot.push_back(job);
            }
            persistence->save_job(snapshot); 
        }
    }

    void load_state()
    {
        if (persistence)
        {
            auto jobs = persistence->load_jobs(); 

            std::lock_guard<std::mutex> lock(mutex);
            for (auto& job : jobs)
            {
                if (job_initializers_.find(job.id) != job_initializers_.end())
                {
                    job_initializers_[job.id](job); 
                }
                else 
                {
                    std::cerr << "[Warning] No initializer found for job " << job.id << "\n";
                }
                jobs[job.id] = job; 
            }
        }
    }

    void register_job_initializer(int job_id, JobInitializer initializer)
    {
        if (initializer)
        {
            std::lock_guard<std::mutex> lock(mutex); 
            job_initializers_[job_id] = initializer; 
        }
    }

private:
    std::unordered_map<int, Job> jobs;
    std::thread worker;
    std::atomic<bool> running;
    std::atomic<int> next_job_id;
    std::mutex mutex;

    ThreadPool thread_pool; 

    std::shared_ptr<Persistence<Job>> persistence; 

    std::unordered_map<int, JobInitializer> job_initializers_;
}; 
