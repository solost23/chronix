#include "example/example4.h"

#include "chronix/chronix.h"
#include "example/printer.h"

/*
 * test callback
 */

static const std::vector<std::pair<std::string, std::function<void()>>> jobs{
    {"*/5 * * * * *",
     []() {
         printer("[任务1] 每5秒执行一次");
         throw std::runtime_error("[任务1] 执行失败");
     }},
    // {"*/8 * * * * *", [](){ printer("[任务2] 每8秒执行一次"); }},
};

static const auto start_callback = [](size_t job_id) {
    printer("任务ID: ", job_id, " 开始执行");
};

static const auto error_callback = [](size_t job_id, const std::exception& e) {
    printer("任务ID: ", job_id, " 执行失败，错误: ", e.what());
};

static const auto success_callback = [](size_t job_id) {
    printer("任务ID: ", job_id, " 执行成功");
};

void example4()
{
    auto scheduler = std::make_shared<ChronixScheduler>(1, 4);

    try
    {
        scheduler->start();

        for (size_t i = 0; i != jobs.size(); i++)
        {
            size_t job_id =
                scheduler->add_cron_job(jobs[i].first, jobs[i].second);
            scheduler->set_start_callback(job_id, start_callback);
            scheduler->set_success_callback(job_id, success_callback);
            scheduler->set_error_callback(job_id, error_callback);
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
