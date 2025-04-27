#include "example/example8.h"

#include "chronix/chronix.h"
#include "example/printer.h"

static const std::vector<std::pair<std::string, std::function<void()>>> jobs{
    {"0 */1 * * * *",
     []() {
         std::this_thread::sleep_for(std::chrono::seconds(1));
         printer("[任务1] 每1分钟执行一次");
     }},  // success
    {"0 */2 * * * *",
     []() {
         std::this_thread::sleep_for(std::chrono::seconds(3));
         printer("[任务2] 每2分钟执行一次");
         throw std::runtime_error("[任务2] 执行失败");
     }},  // failed
    {"0 */3 * * * *",
     []() {
         std::this_thread::sleep_for(std::chrono::seconds(5));
         printer("[任务3] 每3分钟执行一次");
     }},  // paused
};

static const auto format_time = [](std::chrono::system_clock::time_point tp) {
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::ostringstream oss;
    oss << std::put_time(std::localtime(&tt), "%F %T");
    return oss.str();
};

void example8()
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    scheduler->set_metrics_enabled(true);

    static const auto end_callback = [scheduler](size_t job_id) {
        printer("调度器运行状态: ", scheduler->get_running());
        printer("当前的任务总数: ", scheduler->get_job_count());
        printer("当前跑任务总数: ", scheduler->get_running_job_count());

        printer("-----------------------");
        auto metrics = scheduler->get_job_metrics(job_id);
        printer("指标信息-执行次数: ", metrics.execution_count);
        printer("指标信息-成功次叔: ", metrics.success_count);
        printer("指标信息-失败次数: ", metrics.error_count);
        printer("指标信息-最后执行时间: ", format_time(metrics.last_run_time));
        printer("指标信息-最后执行时长: ", metrics.last_duration.count(), "ms");
        printer("指标信息-执行总时长: ", metrics.total_duration.count(), "ms");
        printer("指标信息-最大执行时间: ", metrics.max_duration.count(), "ms");
        printer("指标信息-最小执行时间: ", metrics.min_duration.count(), "ms");
        printer("指标信息-执行平均耗时: ", metrics.average_duration().count(),
                "ms");
        printer("指标信息-执行成功率: ", metrics.success_rate());
        printer("指标信息-执行失败率: ", metrics.error_rate());
        printer("-----------------------");
    };

    try
    {
        scheduler->start();

        for (size_t i = 0; i != jobs.size(); i++)
        {
            size_t job_id =
                scheduler->add_cron_job(jobs[i].first, jobs[i].second);
            scheduler->set_end_callback(job_id, end_callback);
        }
    }
    catch (const std::exception& e)
    {
        printer("scheduler error: ", e.what());
        return;
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
