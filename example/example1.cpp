#include "example/example1.h"

#include "chronix/chronix.h"
#include "example/printer.h"

/*
 * test second
 */

static const std::vector<std::pair<std::string, std::function<void()>>> jobs{
    {"*/1 * * * * *", []() { printer("[任务1] 每1秒执行一次"); }},
    {"*/3 * * * * *", []() { printer("[任务2] 每3秒执行一次"); }},
    {"*/5 * * * * *", []() { printer("[任务3] 每5秒执行一次"); }},
    {"*/7 * * * * *", []() { printer("[任务4] 每7秒执行一次"); }},
    {"*/9 * * * * *", []() { printer("[任务5] 每9秒执行一次"); }},
};

void example1()
{
    auto scheduler = std::make_shared<ChronixScheduler>(1, 4);

    try
    {
        scheduler->start();

        for (size_t i = 0; i != jobs.size(); i++)
        {
            scheduler->add_cron_job(jobs[i].first, jobs[i].second);
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
