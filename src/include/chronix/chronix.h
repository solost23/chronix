#pragma once 

#include <iostream>
#include <mutex>
#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <functional>
#include <unordered_map>
#include <condition_variable>

#include "chronix/define.h" 

#include "chronix/croncpp.h"
#include "chronix/thread_pool/thread_pool.h"
#include "chronix/persistence/persistence.h"


class ChronixScheduler 
{
public:
    ChronixScheduler(size_t thread_count = std::thread::hardware_concurrency()) 
        : running(false), next_job_id(1), thread_pool(thread_count), metrics_enabled(false) {}

    ~ChronixScheduler()
    {
        stop();
    }

    // cron job
    size_t add_cron_job(const std::string& cron_expr, Task task)
    {
        size_t job_id = next_job_id ++;
        cron::cronexpr expr;

        try 
        {
            expr = cron::make_cron(cron_expr);
        }
        catch (const std::exception& e)
        {
            throw std::runtime_error("Invalid cron expression");
        }

        auto next_time = cron::cron_next(expr, std::chrono::system_clock::now());

        {
            std::lock_guard<std::mutex> lock(mutex);
            job_queue.emplace(job_id, next_time);
            job_map.emplace(job_id, Job{
                job_id, 
                expr, 
                cron_expr, 
                std::move(task),  
                next_time, 
                nullptr, 
                nullptr, 
                nullptr, 
                nullptr, 
                JobStatus::Pending,
                JobResult::Unknown, 
                false, 
            });
        }

        cv.notify_one(); 
        return job_id;  
    }

    // add once job
    size_t add_once_job(const std::chrono::system_clock::time_point& run_at, Task task)
    {
        size_t job_id = next_job_id ++;

        {
            std::lock_guard<std::mutex> lock(mutex);
            job_queue.emplace(job_id, run_at);
            job_map.emplace(job_id, Job{
                job_id, 
                {}, 
                "",
                std::move(task), 
                run_at, 
                nullptr,
                nullptr,
                nullptr,
                nullptr, 
                JobStatus::Pending, 
                JobResult::Unknown, 
                true, 
            }); 
        }

        cv.notify_one(); 
        return job_id; 
    }

    void set_start_callback(size_t job_id, StartCallback callback)
    {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = job_map.find(job_id);
        if (it == job_map.end())
        {
            throw std::runtime_error("Job ID " + std::to_string(job_id) + " not found");
        }
        it->second.start_callback = std::move(callback);
    }

    void set_success_callback(size_t job_id, SuccessCallback callback)
    {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = job_map.find(job_id);
        if (it == job_map.end())
        {
            throw std::runtime_error("Job ID " + std::to_string(job_id) + " not found");
        }
        job_map[job_id].success_callback = std::move(callback);
    }

    void set_error_callback(size_t job_id, ErrorCallback callback)
    {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = job_map.find(job_id);
        if (it == job_map.end())
        {
            throw std::runtime_error("Job ID " + std::to_string(job_id) + " not found");
        }
        job_map[job_id].error_callback = std::move(callback);
    }

    void set_end_callback(size_t job_id, EndCallback callback)
    {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = job_map.find(job_id);
        if (it == job_map.end())
        {
            throw std::runtime_error("Job ID " + std::to_string(job_id) + " not found");
        }
        job_map[job_id].end_callback = std::move(callback);
    }

    void set_metrics_enabled(bool enabled)
    {
        metrics_enabled = enabled; 
    }

    // remove job
    void remove_job(size_t job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = job_map.find(job_id);
        if (it == job_map.end())
        {
            throw std::runtime_error("Job ID " + std::to_string(job_id) + " not found");
        }
        // 延时删除任务
        it->second.deleted = true;
    }

    // pause job
    void pause_job(size_t job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = job_map.find(job_id);
        if (it == job_map.end())
        {
            throw std::runtime_error("Job ID " + std::to_string(job_id) + " not found");
        }
        it->second.status = JobStatus::Paused;
    }

    // resume job
    void resume_job(size_t job_id)
    {
        std::lock_guard<std::mutex> lock(mutex);

        auto it = job_map.find(job_id);
        if (it == job_map.end())
        {
            throw std::runtime_error("Job ID " + std::to_string(job_id) + " not found");
        }
        it->second.status = JobStatus::Pending;
    }

    // scheduler start
    void start()
    {
        if (running)
        {
            return ;
        }

        running = true;

        worker = std::thread([this]() {
            while (running)
            {
                std::vector<JobNode> ready_nodes;

                {
                    std::unique_lock<std::mutex> lock(mutex);
                    
                    if (job_queue.empty())
                    {
                        cv.wait_for(lock, std::chrono::milliseconds(100));
                        continue; 
                    }

                    auto now = std::chrono::system_clock::now();
                    while (!job_queue.empty() && now >= job_queue.top().next)
                    {
                        ready_nodes.emplace_back(job_queue.top());
                        job_queue.pop(); 
                    }

                    if (ready_nodes.empty())
                    {
                        auto next_wake = job_queue.top().next;
                        if (next_wake > now)
                        {
                            cv.wait_until(lock, next_wake);
                        }
                        continue; 
                    }
                }

                for (auto& node : ready_nodes)
                {
                    process_job(node);
                }
            }
        }); 
    }

    // // scheduler start
    // void start()
    // {
    //     if (running) 
    //     {
    //         return ;
    //     }

    //     running = true;

    //     worker = std::thread([this](){
    //         while (running)
    //         {
    //             std::unique_lock<std::mutex> lock(mutex); 

    //             if (job_queue.empty())
    //             {
    //                 lock.unlock();

    //                 std::this_thread::sleep_for(std::chrono::milliseconds(100));
    //                 continue; 
    //             }

    //             auto next_node = job_queue.top();

    //             // 延时删除任务
    //             auto it = job_map.find(next_node.id);
    //             bool to_delete = (it == job_map.end() || it->second.deleted);
    //             if (to_delete)
    //             {
    //                 job_queue.pop();

    //                 if (it != job_map.end())
    //                 {
    //                     job_map.erase(it);
    //                 }

    //                 lock.unlock();
    //                 continue; 
    //             }

    //             auto now = std::chrono::system_clock::now();
    //             if (now >= next_node.next)
    //             { 
    //                 job_queue.pop();
    //                 auto& job = job_map[next_node.id]; 

    //                 if (job.status == JobStatus::Pending)
    //                 {
    //                     auto wrapped_task = [this, &job]() {
    //                         auto start_time = std::chrono::system_clock::now();

    //                         try 
    //                         {
    //                             {
    //                                 std::lock_guard<std::mutex> lock(mutex); 
    //                                 job.status = JobStatus::Running; 
    //                             }

    //                             if (job.start_callback)
    //                             {
    //                                 job.start_callback(job.id);
    //                             }

    //                             start_time = std::chrono::system_clock::now();
    //                             job.task(); 
    //                             auto end_time = std::chrono::system_clock::now();

    //                             {
    //                                 std::lock_guard<std::mutex> lock(mutex);
    //                                 job.status = JobStatus::Pending;
    //                                 job.result = JobResult::Success;  
    //                             }

    //                             if (metrics_enabled)
    //                             {
    //                                 std::lock_guard<std::mutex> lock(mutex);
    //                                 std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    //                                 job.metrics.update(true, duration);
    //                             }

    //                             if (job.success_callback)
    //                             {
    //                                 job.success_callback(job.id); 
    //                             }
    //                         }
    //                         catch (const std::exception& e)
    //                         {
    //                             auto end_time = std::chrono::system_clock::now();

    //                             {
    //                                 std::lock_guard<std::mutex> lock(mutex); 
    //                                 job.status = JobStatus::Pending;
    //                                 job.result = JobResult::Failed;  
    //                             }

    //                             if (metrics_enabled)
    //                             {
    //                                 std::lock_guard<std::mutex> lock(mutex);
    //                                 std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    //                                 job.metrics.update(false, duration);
    //                             }

    //                             if (job.error_callback)
    //                             {
    //                                 job.error_callback(job.id, e); 
    //                             }
    //                         }

    //                         if (job.end_callback)
    //                         {
    //                             job.end_callback(job.id); 
    //                         }

    //                         if (job.one_time)
    //                         {
    //                             std::lock_guard<std::mutex> lock(mutex);
    //                             job_map.erase(job.id);
    //                         }
    //                     }; 
    //                     thread_pool.submit(wrapped_task);
    //                 }
    //                 if (!job.one_time)
    //                 {
    //                     job.next = cron::cron_next(job.expr, now);
    //                     job_queue.emplace(job.id, job.next);
    //                 }

    //                 lock.unlock();
    //             }
    //             else 
    //             {
    //                 lock.unlock();
                    
    //                 std::this_thread::sleep_until(next_node.next);
    //             }
    //         }
    //     }); 
    // }

    // scheduler stop
    void stop() 
    {
        running = false;

        cv.notify_all(); 

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
        if (!persistence)
        {
            throw std::runtime_error("No set persistence");
        }

        std::vector<Job> snapshot;

        {
            std::lock_guard<std::mutex> lock(mutex);
            snapshot.reserve(job_map.size());
            for (const auto& [id, job] : job_map)
            {
                snapshot.emplace_back(job);
            }
        }

        persistence->save(snapshot);
    }

    void load_state()
    {
        if (!persistence)
        {
            throw std::runtime_error("No set persistence");
        }

        auto jobs = persistence->load();

        std::vector<std::future<std::optional<std::pair<size_t, Job>>>> futures;
        for (size_t i = 0; i != jobs.size(); i++)
        {
            futures.emplace_back(std::async(std::launch::async, [&, i]() -> std::optional<std::pair<size_t, Job>> {
                Job job = jobs[i];
                auto it = job_initializers_.find(job.id);
                if (it != job_initializers_.end())
                {
                    try {
                        it->second(job);
                        return std::make_pair(job.id, job);
                    }
                    catch (const std::exception& e)
                    {
                        std::cerr << "[Error] Failed to initialize job " << job.id << ": " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cerr << "[Warning] Initializer not found for job " << job.id << std::endl;
                }
                return std::nullopt;
            }));
        }

        std::unordered_map<size_t, Job> local_map;
        std::queue<std::pair<size_t, std::chrono::system_clock::time_point>> local_queue;

        for (auto& f : futures)
        {
            if (auto resp = f.get(); resp.has_value())
            {
                const auto& [id, job] = resp.value();
                local_map[id] = job;
                local_queue.emplace(id, job.next);
            }
        }

        {
            std::lock_guard<std::mutex> lock(mutex);
            for (auto& [id, job] : local_map)
            {
                job_map[id] = std::move(job);
            }
            while (!local_queue.empty())
            {
                const auto& front = local_queue.front();
                job_queue.emplace(front.first, front.second);
                local_queue.pop();
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

        auto it = job_map.find(job_id);
        if (it == job_map.end())
        {
            throw std::runtime_error("Job ID " + std::to_string(job_id) + " not found");
        }
        return it->second.status;
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

        auto it = job_map.find(job_id);
        if (it == job_map.end())
        {
            throw std::runtime_error("Job ID " + std::to_string(job_id) + " not found");
        }
        return it->second.result; 
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

        auto it = job_map.find(job_id);
        if (it == job_map.end())
        {
            throw std::runtime_error("Job ID " + std::to_string(job_id) + " not found");
        }
        return it->second.metrics;
    }

    // get job count
    size_t get_job_count()
    {
        std::lock_guard<std::mutex> lock(mutex);

        size_t count{0};
        for (const auto& [id, job] : job_map)
        {
            if (!job.deleted)
            {
                count ++; 
            }
        }
        return count; 
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
    // process_job
    void process_job(const JobNode& node)
    {
        Job* job_ptr; 

        {
            std::lock_guard<std::mutex> lock(mutex);

            auto it = job_map.find(node.id);
            if (it == job_map.end())
            {
                return ;
            }

            auto& job = it->second; 
            if (job.deleted)
            {
                job_map.erase(it); 
                return ; 
            }

            if (job.status != JobStatus::Pending)
            {
                // 非Peding状态 & 周期性任务 下次继续调度
                if (!job.once)
                {
                    auto calculated_next = job.next;
                    int attempt{0};
                    do 
                    {
                        calculated_next = cron::cron_next(job.expr, calculated_next);
                        attempt ++;
                    } while (calculated_next <= std::chrono::system_clock::now() && attempt < 10); 

                    job.next = calculated_next;
                    job_queue.emplace(job.id, calculated_next); 
                    cv.notify_one(); 
                }
                return ; 
            }

            // 是Pending状态
            job_ptr = &job;
            job_ptr->status = JobStatus::Running; 
            job.status = JobStatus::Running; 
        }

        // 准备任务包装器
        auto wrapped_task = [this, job_ptr]() {
            auto& job = *job_ptr; 
            auto start_time = std::chrono::system_clock::now();

            try {
                if (job.start_callback)
                {
                    job.start_callback(job.id); 
                }

                job.task();

                {
                    std::lock_guard<std::mutex> lock(mutex);

                    job.status = JobStatus::Pending; 
                    job.result = JobResult::Success;

                    if (metrics_enabled)
                    {
                        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time);
                        job.metrics.update(true, duration);
                    }
                }

                if (job.success_callback)
                {
                    job.success_callback(job.id);
                }
            }
            catch(const std::exception& e)
            {
                {
                    std::lock_guard<std::mutex> lock(mutex);

                    job.status = JobStatus::Pending; 
                    job.result = JobResult::Failed;

                    if (metrics_enabled)
                    {
                        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start_time);
                        job.metrics.update(false, duration);
                    }
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

            if (job.once)
            {
                std::lock_guard<std::mutex> lock(mutex);

                job_map.erase(job.id);
            }
            else 
            {
                std::lock_guard<std::mutex> lock(mutex);

                auto it = job_map.find(job.id);
                if (it != job_map.end() && !it->second.deleted)
                {
                    auto calculated_next = job.next; 
                    int attempt{0};
                    do 
                    {
                        calculated_next = cron::cron_next(job.expr, calculated_next);
                        attempt ++;
                    } while (calculated_next <= std::chrono::system_clock::now() && attempt < 10);

                    it->second.next = calculated_next; 
                    job_queue.emplace(job.id, calculated_next);
                    cv.notify_one(); 
                }
            }
        }; 

        thread_pool.submit(wrapped_task);
    }

    std::priority_queue<JobNode, std::vector<JobNode>, std::greater<JobNode>> job_queue; 
    std::unordered_map<size_t, Job> job_map;
    std::thread worker;
    std::atomic<bool> running;
    std::atomic<size_t> next_job_id;
    std::mutex mutex;

    ThreadPool thread_pool; 

    std::shared_ptr<Persistence<Job>> persistence; 

    std::unordered_map<size_t, JobInitializer> job_initializers_;

    std::atomic<bool> metrics_enabled;

    std::condition_variable cv; 
}; 
