#include "example/example5.h"

#include "example/printer.h"
#include "chronix/chronix.h"

/*
 * test file persistence json
 */
void example5()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"*/5 * * * * *", [](){ printer("[任务1] 每5秒执行一次"); }}, 
        {"*/10 * * * * *", [](){ printer("[任务2] 每10秒执行一次"); }}, 
        {"*/20 * * * * *", [](){ printer("[任务3] 每20秒执行一次"); }},
    };

    auto scheduler = std::make_shared<ChronixScheduler>(4);

    scheduler->start();

    scheduler->set_persistence(std::make_shared<FilePersistenceJson<Job>>("./jobs.json"));

    auto error_callback = [](size_t job_id, const std::exception& e) { 
        printer("任务ID: ", job_id, " 执行失败，错误: ", e.what());
    }; 
    auto success_callback = [](size_t job_id) {
        printer("任务ID: ", job_id, " 执行成功");
    };

    for (size_t i = 0; i != jobs.size(); i ++)
    {
        size_t job_id = scheduler->add_cron_job(jobs[i].first, jobs[i].second);
        scheduler->set_success_callback(job_id, success_callback);
        scheduler->set_error_callback(job_id, error_callback);
    }

    scheduler->save_state(); 

    // 注册缺失函数
    scheduler->register_job_initializer(1, [error_callback, success_callback](Job& job) {
        job.task = []() { printer("[任务1] 每5秒执行一次"); };
        job.success_callback = success_callback; 
        job.error_callback = error_callback;
    }); 

    scheduler->register_job_initializer(2, [error_callback, success_callback](Job& job) {
        job.task = []() { printer("[任务2] 每10秒执行一次"); };
        job.success_callback = success_callback; 
        job.error_callback = error_callback;
    }); 

    scheduler->register_job_initializer(3, [error_callback, success_callback](Job& job) {
        job.task = []() { printer("[任务3] 每20秒执行一次"); };
        job.success_callback = success_callback; 
        job.error_callback = error_callback;
    }); 

    scheduler->load_state(); 

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5)); 
    }
}
