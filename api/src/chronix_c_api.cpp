#include "chronix_c_api.h"

#include <chrono>
#include <exception>
#include <memory>
#include <string>

#include "chronix/chronix.h"
#include "chronix/define.h"

extern "C" {
class ChronixPersistenceAdapter : public Persistence<Job>
{
public:
    explicit ChronixPersistenceAdapter(ChronixPersistence* cp) : cp_(cp)
    {}

    std::vector<Job> load() override
    {
        if (cp_ && cp_->load)
        {
            return cp_->load(cp_->persistence_impl);
        }
        return {};
    }

    void save(const std::vector<Job>& jobs) override
    {
        if (cp_ && cp_->save)
        {
            cp_->save(cp_->persistence_impl, jobs);
        }
    }

private:
    ChronixPersistence* cp_;
};

ChronixResp handle_error(int code, const char* message)
{
    ChronixResp resp;
    resp.code = code;
    resp.message = message;
    return resp;
}

ChronixJobIDResp handle_error_with_id(int code, const char* message,
                                      ChronixJobID job_id)
{
    ChronixJobIDResp resp;
    resp.resp = handle_error(code, message);
    resp.job_id = job_id;
    return resp;
}

ChronixJobStatusResp handle_error_with_status(int code, const char* message,
                                              ChronixJobStatus status)
{
    ChronixJobStatusResp resp;
    resp.resp = handle_error(code, message);
    resp.status = status;
    return resp;
}

ChronixJobResultResp handle_error_with_result(int code, const char* message,
                                              ChronixJobResult result)
{
    ChronixJobResultResp resp;
    resp.resp = handle_error(code, message);
    resp.result = result;
    return resp;
}

ChronixJobMetricsResp handle_error_with_metrics(int code, const char* message,
                                                ChronixJobMetrics metrics)
{
    ChronixJobMetricsResp resp;
    resp.resp = handle_error(code, message);
    resp.metrics = metrics;
    return resp;
}

ChronixSchedulerHandle chronix_scheduler_create(ChronixJobID thread_count)
{
    try
    {
        return new ChronixScheduler(thread_count);
    }
    catch (const std::exception& e)
    {
        return nullptr;
    }
}

ChronixResp chronix_scheduler_destroy(ChronixSchedulerHandle handle)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        delete static_cast<ChronixScheduler*>(handle);
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixJobIDResp chronix_scheduler_add_cron_job(ChronixSchedulerHandle handle,
                                                const char* cron_expr,
                                                ChronixJobTaskCallback cb)
{
    if (handle == nullptr)
    {
        return handle_error_with_id(-1, "ChronixSchedulerHandle is null.", 0);
    }

    try
    {
        ChronixJobID job_id =
            static_cast<ChronixScheduler*>(handle)->add_cron_job(
                std::string(cron_expr), [cb]() { cb(); });
        return handle_error_with_id(0, nullptr, job_id);
    }
    catch (const std::exception& e)
    {
        return handle_error_with_id(-2, e.what(), 0);
    }
}

ChronixJobIDResp chronix_scheduler_add_once_job(ChronixSchedulerHandle handle,
                                                long long timestamp_ms,
                                                ChronixJobTaskCallback cb)
{
    if (handle == nullptr)
    {
        return handle_error_with_id(-1, "ChronixSchedulerHandle is null.", 0);
    }

    try
    {
        auto tp = std::chrono::system_clock::time_point(
            std::chrono::milliseconds(timestamp_ms));
        ;
        ChronixJobID job_id =
            static_cast<ChronixScheduler*>(handle)->add_once_job(
                tp, [cb]() { cb(); });
        return handle_error_with_id(0, nullptr, job_id);
    }
    catch (const std::exception& e)
    {
        return handle_error_with_id(-2, e.what(), 0);
    }
}

ChronixJobIDResp chronix_scheduler_add_immediate_job(
    ChronixSchedulerHandle handle, ChronixJobTaskCallback cb)
{
    if (handle == nullptr)
    {
        return handle_error_with_id(-1, "ChronixSchedulerHandle is null.", 0);
    }

    try
    {
        ChronixJobID job_id =
            static_cast<ChronixScheduler*>(handle)->add_immediate_job(
                [cb]() { cb(); });
        return handle_error_with_id(0, nullptr, job_id);
    }
    catch (const std::exception& e)
    {
        return handle_error_with_id(-2, e.what(), 0);
    }
}

ChronixResp chronix_scheduler_set_start_callback(ChronixSchedulerHandle handle,
                                                 ChronixJobID job_id,
                                                 ChronixJobStartCallback cb)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->set_start_callback(
            job_id, [cb](ChronixJobID job_id) { cb(job_id); });
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_set_success_callback(
    ChronixSchedulerHandle handle, ChronixJobID job_id,
    ChronixJobSuccessCallback cb)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->set_success_callback(
            job_id, [cb](ChronixJobID job_id) { cb(job_id); });
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_set_error_callback(ChronixSchedulerHandle handle,
                                                 ChronixJobID job_id,
                                                 ChronixJobErrorCallback cb)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->set_error_callback(
            job_id, [cb](size_t job_id, const std::exception& e) {
                cb(job_id, e.what());
            });
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_set_end_callback(ChronixSchedulerHandle handle,
                                               ChronixJobID job_id,
                                               ChronixJobEndCallback cb)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->set_end_callback(
            job_id, [cb](ChronixJobID job_id) { cb(job_id); });
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_set_metrics_enabled(ChronixSchedulerHandle handle,
                                                  bool enabled)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->set_metrics_enabled(enabled);
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_remove_job(ChronixSchedulerHandle handle,
                                         ChronixJobID job_id)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->remove_job(job_id);
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_pause_job(ChronixSchedulerHandle handle,
                                        ChronixJobID job_id)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->pause_job(job_id);
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_resume_job(ChronixSchedulerHandle handle,
                                         ChronixJobID job_id)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->resume_job(job_id);
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_start(ChronixSchedulerHandle handle)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->start();
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_stop(ChronixSchedulerHandle handle)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->stop();
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_set_persistence(ChronixSchedulerHandle handle,
                                              ChronixPersistence* persistence)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        auto adapter = std::make_shared<ChronixPersistenceAdapter>(persistence);

        static_cast<ChronixScheduler*>(handle)->set_persistence(adapter);
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_save_immediately(ChronixSchedulerHandle handle,
                                               ChronixJobID job_id)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->save_immediately(job_id);
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_save_periodically(ChronixSchedulerHandle handle)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->save_periodically();
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chronix_scheduler_load_state(ChronixSchedulerHandle handle)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->load_state();
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixResp chonix_scheduler_register_job_initializer(
    ChronixSchedulerHandle handle, ChronixJobID job_id,
    ChronixJobInitializer initializer)
{
    if (handle == nullptr)
    {
        return handle_error(-1, "ChronixSchedulerHandle is null.");
    }

    try
    {
        static_cast<ChronixScheduler*>(handle)->register_job_initializer(
            job_id, [initializer](Job& job) {
                initializer(reinterpret_cast<ChronixJob*>(&job));
            });
        return handle_error(0, nullptr);
    }
    catch (const std::exception& e)
    {
        return handle_error(-2, e.what());
    }
}

ChronixJobStatusResp chronix_scheduler_get_job_status(
    ChronixSchedulerHandle handle, ChronixJobID job_id)
{
    if (handle == nullptr)
    {
        return handle_error_with_status(-1, "ChronixSchedulerHandle is null.",
                                        ChronixJobStatus::CHRONIX_JOB_PENDING);
    }

    try
    {
        ChronixJobStatus status = static_cast<ChronixJobStatus>(
            static_cast<ChronixScheduler*>(handle)->get_job_status(job_id));
        return handle_error_with_status(0, nullptr, status);
    }
    catch (const std::exception& e)
    {
        return handle_error_with_status(-2, e.what(),
                                        ChronixJobStatus::CHRONIX_JOB_PENDING);
    }
}

const char* status_to_string(ChronixJobStatus status)
{
    switch (status)
    {
    case ChronixJobStatus::CHRONIX_JOB_PENDING:
        return "Pending";
    case ChronixJobStatus::CHRONIX_JOB_RUNNING:
        return "Running";
    case ChronixJobStatus::CHRONIX_JOB_PAUSED:
        return "Paused";
    default:
        return "Unknown";
    }
}

ChronixJobResultResp chronix_scheduler_get_job_result(
    ChronixSchedulerHandle handle, ChronixJobID job_id)
{
    if (handle == nullptr)
    {
        return handle_error_with_result(
            -1, "ChronixSchedulerHandle is null.",
            ChronixJobResult::CHRONIX_RESULT_UNKNOWN);
    }

    try
    {
        ChronixJobResult result = static_cast<ChronixJobResult>(
            static_cast<ChronixScheduler*>(handle)->get_job_result(job_id));
        return handle_error_with_result(0, nullptr, result);
    }
    catch (std::exception& e)
    {
        return handle_error_with_result(
            -2, e.what(), ChronixJobResult::CHRONIX_RESULT_UNKNOWN);
    }
}

const char* result_to_string(ChronixJobResult result)
{
    switch (result)
    {
    case ChronixJobResult::CHRONIX_RESULT_SUCCESS:
        return "Success";
    case ChronixJobResult::CHRONIX_RESULT_FAILED:
        return "Failed";
    default:
        return "Unknown";
    }
}

ChronixJobMetricsResp chronix_scheduler_get_job_metrics(
    ChronixSchedulerHandle handle, ChronixJobID job_id)
{
    if (handle == nullptr)
    {
        return handle_error_with_metrics(-1, "ChronixSchedulerHandle is null.",
                                         ChronixJobMetrics{});
    }

    try
    {
        JobMetrics metrics =
            static_cast<ChronixScheduler*>(handle)->get_job_metrics(job_id);

        ChronixJobMetrics resp;
        resp.execution_count = metrics.execution_count;
        resp.success_count = metrics.success_count;
        resp.error_count = metrics.error_count;
        resp.last_duration_ms = metrics.last_duration.count();
        resp.total_duration_ms = metrics.total_duration.count();
        resp.max_duration_ms = metrics.max_duration.count();
        resp.min_duration_ms = metrics.min_duration.count();
        resp.success_rate = metrics.success_rate();
        resp.error_rate = metrics.error_rate();

        return handle_error_with_metrics(0, nullptr, resp);
    }
    catch (std::exception& e)
    {
        return handle_error_with_metrics(-2, e.what(), ChronixJobMetrics{});
    }
}

ChronixJobIDResp chronix_scheduler_get_job_count(ChronixSchedulerHandle handle)
{
    if (handle == nullptr)
    {
        return handle_error_with_id(-1, "ChronixSchedulerHandle is null.", 0);
    }

    try
    {
        ChronixJobID count =
            static_cast<ChronixScheduler*>(handle)->get_job_count();
        return handle_error_with_id(0, nullptr, count);
    }
    catch (std::exception& e)
    {
        return handle_error_with_id(-2, e.what(), 0);
    }
}

ChronixJobIDResp chronix_scheduler_get_running_job_count(
    ChronixSchedulerHandle handle)
{
    if (handle == nullptr)
    {
        return handle_error_with_id(-1, "ChronixSchedulerHandle is null.", 0);
    }

    try
    {
        ChronixJobID count =
            static_cast<ChronixScheduler*>(handle)->get_running_job_count();
        return handle_error_with_id(0, nullptr, count);
    }
    catch (std::exception& e)
    {
        return handle_error_with_id(-2, e.what(), 0);
    }
}

bool chronix_scheduler_get_running(ChronixSchedulerHandle handle)
{
    if (handle == nullptr)
    {
        throw std::invalid_argument("ChronixSchedulerHandle is null.");
    }

    return static_cast<ChronixScheduler*>(handle)->get_running();
}

}  // extern "C"