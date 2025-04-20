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

struct Job
{
    int id;
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
};

struct JobNode 
{
    int id;
    std::chrono::system_clock::time_point next;

    bool operator>(const JobNode& other) const 
    {
        return next > other.next;
    }
}; 

using JobInitializer = std::function<void(Job&)>;
