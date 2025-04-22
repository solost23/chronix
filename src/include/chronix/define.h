#pragma once

#include <functional> 
#include "chronix/croncpp.h"

using Task = std::function<void()>;

using StartCallback = std::function<void(int job_id)>;
using EndCallback = std::function<void(int job_id)>;
using ErrorCallback = std::function<void(int job_id, const std::exception& e)>;
using SuccessCallback = std::function<void(int job_id)>; 

enum class JobStatus
{
    Pending,
    Running,
    Paused,
}; 

enum class JobResult
{
    Unknown,
    Success,
    Failed
}; 

struct JobMetrics {
    size_t execution_count{0};
    size_t success_count{0};
    size_t error_count{0};

    std::chrono::system_clock::time_point last_run_time{};
    std::chrono::milliseconds last_duration{0};
    std::chrono::milliseconds total_duration{0};

    std::chrono::milliseconds max_duration{0}; 
    std::chrono::milliseconds min_duration{std::chrono::milliseconds::max()}; 

    std::vector<std::chrono::system_clock::time_point> success_times;
    std::vector<std::chrono::system_clock::time_point> error_times;

    std::string description;

    void update(bool success, std::chrono::milliseconds duration)
    {
        execution_count ++;

        if (success)
        {
            success_count ++;
            success_times.emplace_back(std::chrono::system_clock::now());
        }
        else 
        {
            error_count ++;
            error_times.emplace_back(std::chrono::system_clock::now());
        }

        last_run_time = std::chrono::system_clock::now();
        last_duration = duration;
        total_duration += duration;

        if (duration > max_duration) {
            max_duration = duration;
        }
        if (duration < min_duration)
        {
            min_duration = duration; 
        }
    }

    std::chrono::milliseconds average_duration() const 
    {
        return execution_count > 0 
            ? std::chrono::duration_cast<std::chrono::milliseconds>(total_duration / execution_count) 
            : std::chrono::milliseconds{0};
    }; 

    double success_rate() const 
    {
        size_t total = execution_count;
        return total > 0 ? static_cast<double>(success_count) / total : 0.0;
    }

    double error_rate() const 
    {
        size_t total = execution_count;
        return total > 0 ? static_cast<double>(error_count) / total : 0.0;
    }
};

struct Job
{
    size_t id;
    cron::cronexpr expr;
    std::string expr_str; 
    Task task;
    std::chrono::system_clock::time_point next;

    ErrorCallback error_callback;
    SuccessCallback success_callback; 
    StartCallback start_callback;
    EndCallback end_callback;

    JobStatus status;
    JobResult result;

    bool one_time;

    JobMetrics metrics;

    bool deleted{false}; 
};

struct JobNode 
{
    size_t id;
    std::chrono::system_clock::time_point next;

    bool operator>(const JobNode& other) const 
    {
        return next > other.next;
    }
}; 

using JobInitializer = std::function<void(Job&)>;
