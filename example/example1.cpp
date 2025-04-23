#include "example/example1.h"

#include "example/printer.h"
#include "chronix/chronix.h"


/*
 * test second
 */
void example1()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"*/1 * * * * *", [](){ printer("[任务1] 每1秒执行一次"); }}, 
        {"*/3 * * * * *", [](){ printer("[任务2] 每3秒执行一次"); }}, 
        {"*/5 * * * * *", [](){ printer("[任务3] 每5秒执行一次"); }},
        {"*/7 * * * * *", [](){ printer("[任务4] 每7秒执行一次"); }}, 
        {"*/9 * * * * *", [](){ printer("[任务5] 每9秒执行一次"); }},  
    }; 

    auto scheduler = std::make_shared<ChronixScheduler>(4); 

    scheduler->start();

    for (size_t i = 0; i != jobs.size(); i ++)
    {
        scheduler->add_cron_job(jobs[i].first, jobs[i].second);
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
