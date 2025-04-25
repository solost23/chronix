#include "example/example7.h"

#include "chronix/chronix.h"
#include "example/printer.h"

/*
 * test task status and result
 */
void example7()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"*/3 * * * * *",
         []() { printer("[任务1] 每3秒执行一次"); }},  // success
        {"*/5 * * * * *",
         []() {
             printer("[任务2] 每5秒执行一次");
             throw std::runtime_error("[任务2] 执行失败");
         }},  // failed
        {"*/7 * * * * *",
         []() { printer("[任务3] 每7秒执行一次"); }},  // paused
    };

    auto scheduler = std::make_shared<ChronixScheduler>(4);

    scheduler->start();

    scheduler->set_persistence(std::make_shared<DBPersistenceMySQL<Job>>(
        "mysql", 33060, "root", "123", "chronix"));

    // TODO: 任务结束钩子，持久化快照, 暂时全量更新
    auto end_callback = [scheduler](size_t job_id) {
        scheduler->save_state();
        printer("任务[", job_id, "] 保存成功");
    };

    for (size_t i = 0; i != jobs.size(); i++)
    {
        size_t job_id = scheduler->add_cron_job(jobs[i].first, jobs[i].second);
        scheduler->set_end_callback(job_id, end_callback);
    }

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 第三个任务暂停，暂停后手动保存
    scheduler->pause_job(3);
    // 保存结果到文件中
    scheduler->save_state();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
