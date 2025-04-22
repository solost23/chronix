#include "example/example2.h"

#include "example/printer.h"
#include "chronix/chronix.h"

/*
 * test minute
 */
void example2()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"0 */1 * * * *", [](){ printer("[任务1] 每1分执行一次"); }}, 
        {"0 */3 * * * *", [](){ printer("[任务2] 每3分执行一次"); }}, 
        {"0 */5 * * * *", [](){ printer("[任务3] 每5分执行一次"); }},
        {"0 */7 * * * *", [](){ printer("[任务4] 每7分执行一次"); }}, 
        {"0 */9 * * * *", [](){ printer("[任务5] 每9分执行一次"); }},  
    }; 

    auto scheduler = std::make_shared<ChronixScheduler>(4); 

    for (size_t i = 0; i != jobs.size(); i ++)
    {
        scheduler->add_cron_job(jobs[i].first, jobs[i].second);
    }

    scheduler->start();  

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }  
}