#pragma once 

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <unordered_map>

#include "chronix/define.h" 

#include "chronix/croncpp.h"
#include "chronix/thread_pool/thread_pool.h"
#include "chronix/persistence/persistence.h"


class ChronixScheduler 
{
public:
    ChronixScheduler(size_t thread_count = std::thread::hardware_concurrency()) 
        : running(false), next_job_id(1), thread_pool(thread_count) {}

    ~ChronixScheduler()
    {
        stop();
    }

    // task add
    int add_job(
        const std::string& cron_expr, 
        Task task, 
        ErrorCallback error_callback = nullptr, 
        SuccessCallback success_callback = nullptr, 
        StartCallback start_callback = nullptr
    )
    {
        std::lock_guard<std::mutex> lock(mutex);

        cron::cronexpr expr = cron::make_cron(cron_expr);
        int job_id = next_job_id ++;
        auto next_time = cron::cron_next(expr, std::chrono::system_clock::now()); 

        job_queue.push({job_id, next_time});
        job_map[job_id] = Job{
            job_id, 
            expr, 
            cron_expr, 
            task,  
            next_time, 
            false, 
            error_callback, 
            success_callback, 
            start_callback
        };

        return job_id; 
    }

    // task remove
    void remove_job(int job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (job_map.count(job_id))
        {
            job_map.erase(job_id); 
        }
    }

    // task stop
    void pause_job(int job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (job_map.count(job_id))
        {
            job_map[job_id].paused = true;
        }
    }

    // task resume
    void resume_job(int job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (job_map.count(job_id))
        {
            job_map[job_id].paused = false; 
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
                std::unique_lock<std::mutex> lock(mutex); 

                if (job_queue.empty())
                {
                    lock.unlock();

                    std::this_thread::sleep_for(std::chrono::milliseconds(100));
                    continue; 
                }

                JobNode next_node = job_queue.top();

                if (!job_map.count(next_node.id))
                {
                    job_queue.pop();

                    lock.unlock();
                    continue; 
                }

                auto now = std::chrono::system_clock::now();
                if (now >= next_node.next)
                { 
                    job_queue.pop();
                    auto& job = job_map[next_node.id]; 

                    if (!job.paused)
                    {
                        auto wrapped_task = [this, job]() {
                            try 
                            {
                                if (job.start_callback)
                                {
                                    job.start_callback(job.id);
                                }
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
                    }
                    
                    job.next = cron::cron_next(job.expr, now);
                    job_queue.push({job.id, job.next});

                    lock.unlock();
                }
                else 
                {
                    lock.unlock();
                    
                    std::this_thread::sleep_until(next_node.next);
                }
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
            for (auto& [id, job] : job_map)
            {
                snapshot.push_back(job);
            }
            persistence->save(snapshot); 
        }
    }

    void load_state()
    {
        if (persistence)
        {
            auto jobs = persistence->load(); 

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
                job_map[job.id] = job; 
                job_queue.push({job.id, job.next});
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
    std::priority_queue<JobNode, std::vector<JobNode>, std::greater<JobNode>> job_queue; 
    std::unordered_map<int, Job> job_map;
    std::thread worker;
    std::atomic<bool> running;
    std::atomic<int> next_job_id;
    std::mutex mutex;

    ThreadPool thread_pool; 

    std::shared_ptr<Persistence<Job>> persistence; 

    std::unordered_map<int, JobInitializer> job_initializers_;
}; 
