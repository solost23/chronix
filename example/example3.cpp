#include "example/example3.h"

#include "chronix/chronix.h"
#include "example/printer.h"

/*
 * test
 */

static const std::vector<std::pair<std::string, std::function<void()>>> jobs{
    {"0 15 0 * * *", []() { printer("[任务1] 每天00:15执行一次"); }},
    {"0 16 0 * * *", []() { printer("[任务2] 每天00:16执行一次"); }},
    {"0 17 0 * * *", []() { printer("[任务3] 每天00:17执行一次"); }},
    {"0 18 0 * * *", []() { printer("[任务4] 每天00:18执行一次"); }},
    {"0 19 0 * * *", []() { printer("[任务5] 每天00:19执行一次"); }},
};

void example3()
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