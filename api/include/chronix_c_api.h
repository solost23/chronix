#pragma once

#include <stdbool.h>
#include <stdint.h>

#include <vector>

#include "chronix/define.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef void* ChronixSchedulerHandle;
typedef uint64_t ChronixJobID;
typedef int64_t ChronixJobTimestamp;

typedef void (*ChronixJobTaskCallback)(void);
typedef void (*ChronixJobStartCallback)(ChronixJobID job_id);
typedef void (*ChronixJobEndCallback)(ChronixJobID job_id);
typedef void (*ChronixJobErrorCallback)(ChronixJobID job_id,
                                        const char* error_message);
typedef void (*ChronixJobSuccessCallback)(ChronixJobID job_id);

typedef enum
{
    CHRONIX_JOB_PENDING,
    CHRONIX_JOB_RUNNING,
    CHRONIX_JOB_PAUSED
} ChronixJobStatus;

typedef enum
{
    CHRONIX_RESULT_UNKNOWN,
    CHRONIX_RESULT_SUCCESS,
    CHRONIX_RESULT_FAILED
} ChronixJobResult;

typedef enum
{
    CHRONIX_TYPE_CRON,
    CHRONIX_TYPE_ONCE,
    CHRONIX_TYPE_IMMEDIATE
} ChronixJobType;

typedef struct
{
    uint64_t execution_count;
    uint64_t success_count;
    uint64_t error_count;

    ChronixJobTimestamp last_run_time;
    int64_t last_duration_ms;
    int64_t total_duration_ms;
    int64_t max_duration_ms;
    int64_t min_duration_ms;

    double success_rate;
    double error_rate;

    const char* description;
} ChronixJobMetrics;

typedef struct
{
    ChronixJobID id;
    ChronixJobTimestamp next_time;

    ChronixJobTaskCallback task;
    ChronixJobStartCallback on_start;
    ChronixJobEndCallback on_end;
    ChronixJobSuccessCallback on_success;
    ChronixJobErrorCallback on_error;

    ChronixJobStatus status;
    ChronixJobResult result;
    ChronixJobType type;

    ChronixJobMetrics metrics;
    bool deleted;
} ChronixJob;

typedef void (*ChronixJobInitializer)(ChronixJob* job);

typedef struct
{
    int code;             // 0=success, -1=invalid handle, -2=error
    const char* message;  // optonal error message
} ChronixResp;

typedef struct
{
    ChronixResp resp;
    ChronixJobID job_id;
} ChronixJobIDResp;

typedef struct
{
    ChronixResp resp;
    ChronixJobStatus status;
} ChronixJobStatusResp;

typedef struct
{
    ChronixResp resp;
    ChronixJobResult result;
} ChronixJobResultResp;

typedef struct
{
    ChronixResp resp;
    ChronixJobMetrics metrics;
} ChronixJobMetricsResp;

typedef struct
{
    void* persistence_impl;  // 指向用户自定义实现的指针
    std::vector<Job> (*load)(void* persistence_impl);  // load 函数的回调
    void (*save)(void* persistence_impl,
                 const std::vector<Job>& jobs);  // save 函数的回调
} ChronixPersistence;

// 基础错误处理 (2个参数)
inline ChronixResp handle_error(int code, const char* message);

// 带额外数据的错误处理 (使用不同函数名避免重载冲突)
inline ChronixJobIDResp handle_error_with_id(int code, const char* message,
                                             ChronixJobID job_id);
inline ChronixJobStatusResp handle_error_with_status(int code,
                                                     const char* message,
                                                     ChronixJobStatus status);
inline ChronixJobResultResp handle_error_with_result(int code,
                                                     const char* message,
                                                     ChronixJobResult result);
inline ChronixJobMetricsResp handle_error_with_metrics(
    int code, const char* message, ChronixJobMetrics metrics);

// Function declarations
ChronixSchedulerHandle chronix_scheduler_create(ChronixJobID thread_count);
ChronixResp chronix_scheduler_destroy(ChronixSchedulerHandle handle);
ChronixJobIDResp chronix_scheduler_add_cron_job(ChronixSchedulerHandle handle,
                                                const char* cron_expr,
                                                ChronixJobTaskCallback cb);
ChronixJobIDResp chronix_scheduler_add_once_job(ChronixSchedulerHandle handle,
                                                long long timestamp_ms,
                                                ChronixJobTaskCallback cb);
ChronixJobIDResp chronix_scheduler_add_immediate_job(
    ChronixSchedulerHandle handle, ChronixJobTaskCallback cb);
ChronixResp chronix_scheduler_set_start_callback(ChronixSchedulerHandle handle,
                                                 ChronixJobID job_id,
                                                 ChronixJobStartCallback cb);
ChronixResp chronix_scheduler_set_success_callback(
    ChronixSchedulerHandle handle, ChronixJobID job_id,
    ChronixJobSuccessCallback cb);
ChronixResp chronix_scheduler_set_error_callback(ChronixSchedulerHandle handle,
                                                 ChronixJobID job_id,
                                                 ChronixJobErrorCallback cb);
ChronixResp chronix_scheduler_set_end_callback(ChronixSchedulerHandle handle,
                                               ChronixJobID job_id,
                                               ChronixJobEndCallback cb);
ChronixResp chronix_scheduler_set_metrics_enabled(ChronixSchedulerHandle handle,
                                                  bool enabled);
ChronixResp chronix_scheduler_remove_job(ChronixSchedulerHandle handle,
                                         ChronixJobID job_id);
ChronixResp chronix_scheduler_pause_job(ChronixSchedulerHandle handle,
                                        ChronixJobID job_id);
ChronixResp chronix_scheduler_resume_job(ChronixSchedulerHandle handle,
                                         ChronixJobID job_id);
ChronixResp chronix_scheduler_start(ChronixSchedulerHandle handle);
ChronixResp chronix_scheduler_stop(ChronixSchedulerHandle handle);
ChronixResp chronix_scheduler_set_persistence(
    ChronixSchedulerHandle handle, ChronixPersistence* persistence_backend);
ChronixResp chronix_scheduler_save_immediately(ChronixSchedulerHandle handle,
                                               ChronixJobID job_id);
ChronixResp chronix_scheduler_save_periodically(ChronixSchedulerHandle handle);
ChronixResp chronix_scheduler_load_state(ChronixSchedulerHandle handle);
ChronixResp chonix_scheduler_register_job_initializer(
    ChronixSchedulerHandle handle, ChronixJobID job_id,
    ChronixJobInitializer initializer);
ChronixJobStatusResp chronix_scheduler_get_job_status(
    ChronixSchedulerHandle handle, ChronixJobID job_id);
const char* status_to_string(ChronixJobStatus status);
ChronixJobResultResp chronix_scheduler_get_job_result(
    ChronixSchedulerHandle handle, ChronixJobID job_id);
const char* result_to_string(ChronixJobResult result);
ChronixJobMetricsResp chronix_scheduler_get_job_metrics(
    ChronixSchedulerHandle handle, ChronixJobID job_id);
ChronixJobIDResp chronix_scheduler_get_job_count(ChronixSchedulerHandle handle);
ChronixJobIDResp chronix_scheduler_get_running_job_count(
    ChronixSchedulerHandle handle);
bool chronix_scheduler_get_running(ChronixSchedulerHandle handle);

#ifdef __cplusplus
}
#endif
