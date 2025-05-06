#include "example/example6.h"

#include "chronix/chronix.h"
#include "example/printer.h"

/*
 * test db persistence mysql
 * test task status and result
 */

static const std::vector<std::pair<std::string, std::function<void()>>> jobs{
    {"*/5 * * * * *", []() { printer("[任务1] 每5秒执行一次"); }},  // success
    {"*/10 * * * * *",
     []() {
         printer("[任务2] 每10秒执行一次");
         throw std::runtime_error("[任务2] 执行失败");
     }},                                                              // failed
    {"*/20 * * * * *", []() { printer("[任务3] 每20秒执行一次"); }},  // paused
    {"*/30 * * * * *", []() { printer("[任务4] 每30秒执行一次"); }},  // success
};

static const auto error_callback = [](size_t job_id, const std::exception& e) {
    printer("任务ID: ", job_id, " 执行失败，错误: ", e.what());
};

static const auto success_callback = [](size_t job_id) {
    printer("任务ID: ", job_id, " 执行成功");
};

void example6()
{
    auto scheduler = std::make_shared<ChronixScheduler>(1, 4);

    scheduler->set_persistence(std::make_shared<DBPersistenceMySQL<Job>>(
        "localhost", 33036, "root", "123", "chronix"));

    // TODO: 任务结束钩子，持久化数据
    static const auto end_callback = [scheduler](size_t job_id) {
        if (scheduler)
        {
            scheduler->save_immediately(job_id);
        }
        else
        {
            printer("scheduler error: weak_scheduler is expired");
        }
    };

    try
    {
        scheduler->start();

        for (size_t i = 0; i != jobs.size(); i++)
        {
            size_t job_id =
                scheduler->add_cron_job(jobs[i].first, jobs[i].second);
            scheduler->set_success_callback(job_id, success_callback);
            scheduler->set_error_callback(job_id, error_callback);
            scheduler->set_end_callback(job_id, end_callback);

            // 注册缺失函数
            scheduler->register_job_initializer(
                job_id, [task = jobs[i].second](Job& job) {
                    job.task = task;
                    job.success_callback = success_callback;
                    job.error_callback = error_callback;
                });
        }

        std::this_thread::sleep_for(std::chrono::seconds(15));
        // 任务暂停
        scheduler->pause_job(3);

        // 定时持久化
        size_t count{0};
        while (count != 11)
        {
            scheduler->save_periodically();
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }

        // 加载
        scheduler->load_state();
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
