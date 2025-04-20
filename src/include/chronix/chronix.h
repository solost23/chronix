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

    // cron job
    size_t add_cron_job(const std::string& cron_expr, Task task)
    {
        std::lock_guard<std::mutex> lock(mutex);

        cron::cronexpr expr = cron::make_cron(cron_expr);
        size_t job_id = next_job_id ++;
        auto next_time = cron::cron_next(expr, std::chrono::system_clock::now()); 

        job_queue.push({job_id, next_time});
        job_map[job_id] = Job{
            job_id, 
            expr, 
            cron_expr, 
            task,  
            next_time, 
            nullptr, 
            nullptr, 
            nullptr, 
            nullptr, 
            JobStatus::Pending,
            JobResult::Unknown, 
            false,  
        };

        return job_id; 
    }

    // one time job
    size_t add_one_time_job(const std::chrono::system_clock::time_point& run_at, Task task)
    {
        std::lock_guard<std::mutex> lock(mutex);

        size_t job_id = next_job_id ++;

        job_queue.push({job_id, run_at});
        job_map[job_id] = Job{
            job_id, 
            {}, 
            "",
            task, 
            run_at, 
            nullptr,
            nullptr,
            nullptr,
            nullptr, 
            JobStatus::Pending, 
            JobResult::Unknown, 
            true, 
        }; 

        return job_id;
    }

    void set_start_callback(size_t job_id, StartCallback callback)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!job_map.count(job_id))
        {
            throw std::runtime_error("Job ID not found");
        }
        job_map[job_id].start_callback = callback;
    }

    void set_success_callback(size_t job_id, SuccessCallback callback)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!job_map.count(job_id))
        {
            throw std::runtime_error("Job ID not found");
        }
        job_map[job_id].success_callback = callback;
    }

    void set_error_callback(size_t job_id, ErrorCallback callback)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!job_map.count(job_id))
        {
            throw std::runtime_error("Job ID not found");
        }
        job_map[job_id].error_callback = callback;
    }

    void set_end_callback(size_t job_id, EndCallback callback)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!job_map.count(job_id))
        {
            throw std::runtime_error("Job ID not found");
        }
        job_map[job_id].end_callback = callback;
    }

    // remove job
    void remove_job(size_t job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!job_map.count(job_id))
        {
            throw std::runtime_error("Job ID not found");
        }
        job_map.erase(job_id); 
    }

    // pause job
    void pause_job(size_t job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!job_map.count(job_id))
        {
            throw std::runtime_error("Job ID not found");
        }
        job_map[job_id].status = JobStatus::Paused; 
    }

    // resume job
    void resume_job(size_t job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!job_map.count(job_id))
        {
            throw std::runtime_error("Job ID not found");
        }
        job_map[job_id].status = JobStatus::Pending; 
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

                    if (job.status == JobStatus::Pending)
                    {
                        auto wrapped_task = [this, &job]() {
                            auto start_time = std::chrono::system_clock::now();

                            try 
                            {
                                {
                                    std::lock_guard<std::mutex> lock(mutex); 
                                    job.status = JobStatus::Running; 
                                }

                                if (job.start_callback)
                                {
                                    job.start_callback(job.id);
                                }

                                start_time = std::chrono::system_clock::now();
                                job.task(); 
                                auto end_time = std::chrono::system_clock::now();

                                {
                                    std::lock_guard<std::mutex> lock(mutex);
                                    job.status = JobStatus::Pending;
                                    job.result = JobResult::Success;  
                                }

                                {
                                    std::lock_guard<std::mutex> lock(mutex);
                                    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                                    job.metrics.update(true, duration);
                                }

                                if (job.success_callback)
                                {
                                    job.success_callback(job.id); 
                                }
                            }
                            catch (const std::exception& e)
                            {
                                auto end_time = std::chrono::system_clock::now();

                                {
                                    std::lock_guard<std::mutex> lock(mutex); 
                                    job.status = JobStatus::Pending;
                                    job.result = JobResult::Failed;  
                                }

                                {
                                    std::lock_guard<std::mutex> lock(mutex);
                                    std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
                                    job.metrics.update(true, duration);
                                }

                                if (job.error_callback)
                                {
                                    job.error_callback(job.id, e); 
                                }
                            }

                            if (job.end_callback)
                            {
                                job.end_callback(job.id); 
                            }

                            if (job.one_time)
                            {
                                std::lock_guard<std::mutex> lock(mutex);
                                job_map.erase(job.id);
                            }
                        }; 
                        thread_pool.submit(wrapped_task);
                    }
                    if (!job.one_time)
                    {
                        job.next = cron::cron_next(job.expr, now);
                        job_queue.push({job.id, job.next});
                    }

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

    void register_job_initializer(size_t job_id, JobInitializer initializer)
    {
        if (initializer)
        {
            std::lock_guard<std::mutex> lock(mutex); 
            job_initializers_[job_id] = initializer; 
        }
    }

    // get job status
    JobStatus get_job_status(size_t job_id)
    {
        std::lock_guard<std::mutex> lock(mutex); 

        if (!job_map.count(job_id))
        {
            throw std::runtime_error("Job ID not found");
        }
        return job_map[job_id].status;
    }

    // status to string
    std::string status_to_string(JobStatus status)
    {
        switch(status)
        {
            case JobStatus::Pending: return "Pending";
            case JobStatus::Running: return "Running";
            case JobStatus::Paused: return "Paused";
        }
    }

    // get job last result
    JobResult get_job_result(size_t job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!job_map.count(job_id))
        {
            throw std::runtime_error("Job ID not found");
        }
        return job_map[job_id].result; 
    }

    // result to string
    std::string result_to_string(JobResult result)
    {
        switch(result)
        {
            case JobResult::Success: return "Success";
            case JobResult::Failed: return "Failed";
            case JobResult::Unknown: return "Unknown"; 
        }
    }

    // get job metrics
    JobMetrics get_job_metrics(size_t job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        if (!job_map.count(job_id))
        {
            throw std::runtime_error("Job ID not found");
        }

        return job_map[job_id].metrics;
    }

    // get job count
    size_t get_job_count()
    {
        std::lock_guard<std::mutex> lock(mutex);

        return job_map.size();
    }

    // get running job count
    size_t get_running_job_count()
    {
        std::lock_guard<std::mutex> lock(mutex);

        size_t count{0};
        for (const auto& [id, job] : job_map)
        {
            if (job.status == JobStatus::Running)
            {
                count ++;
            }
        }

        return count; 
    }

    // get running
    size_t get_running() const 
    {
        return running; 
    }

private:
    std::priority_queue<JobNode, std::vector<JobNode>, std::greater<JobNode>> job_queue; 
    std::unordered_map<size_t, Job> job_map;
    std::thread worker;
    std::atomic<bool> running;
    std::atomic<size_t> next_job_id;
    std::mutex mutex;

    ThreadPool thread_pool; 

    std::shared_ptr<Persistence<Job>> persistence; 

    std::unordered_map<size_t, JobInitializer> job_initializers_;
}; 
