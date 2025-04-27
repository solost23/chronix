#include "example/example5.h"

#include "chronix/chronix.h"
#include "example/printer.h"

/*
 * test file persistence json
 */

static const std::vector<std::pair<std::string, std::function<void()>>> jobs{
    {"*/5 * * * * *", []() { printer("[任务1] 每5秒执行一次"); }},
    {"*/10 * * * * *", []() { printer("[任务2] 每10秒执行一次"); }},
    {"*/15 * * * * *", []() { printer("[任务3] 每15秒执行一次"); }},
    {"*/20 * * * * *", []() { printer("[任务4] 每20秒执行一次"); }},
};

static const auto error_callback = [](size_t job_id, const std::exception& e) {
    printer("任务ID: ", job_id, " 执行失败，错误: ", e.what());
};

static const auto success_callback = [](size_t job_id) {
    printer("任务ID: ", job_id, " 执行成功");
};

void example5()
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    scheduler->set_persistence(
        std::make_shared<FilePersistenceJson<Job>>("./jobs.json"));

    try
    {
        scheduler->start();

        for (size_t i = 0; i != jobs.size(); i++)
        {
            size_t job_id =
                scheduler->add_cron_job(jobs[i].first, jobs[i].second);
            scheduler->set_success_callback(job_id, success_callback);
            scheduler->set_error_callback(job_id, error_callback);

            // 注册缺失函数
            scheduler->register_job_initializer(
                job_id, [task = jobs[i].second](Job& job) {
                    job.task = task;
                    job.success_callback = success_callback;
                    job.error_callback = error_callback;
                });
        }

        // 持久化
        scheduler->save_state();
        // 加载
        scheduler->load_state();
    }
    catch (const std::exception& e)
    {
        printer("scheduler error: ", e.what());
        return;
    }

    scheduler->save_state();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
