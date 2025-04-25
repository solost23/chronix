#include "example/example10.h"

#include "chronix/chronix.h"
#include "example/printer.h"

/*
 * immediate
 */
void example10()
{
    std::vector<std::function<void()>> jobs{
        []() { printer("[任务1] 一次性任务"); },
        []() { printer("[任务2] 一次性任务"); },
        []() { printer("[任务3] 一次性任务"); },
        []() { printer("[任务4] 一次性任务"); },
        []() { printer("[任务5] 一次性任务"); },
        []() { printer("[任务6] 一次性任务"); },
        []() { printer("[任务7] 一次性任务"); },
        []() { printer("[任务8] 一次性任务"); },
        []() { printer("[任务9] 一次性任务"); },
        []() { printer("[任务10] 一次性任务"); },
    }; 

    auto scheduler = std::make_shared<ChronixScheduler>(4);

    scheduler->start();

    for (size_t i = 0; i != jobs.size(); i++)
    {
        scheduler->add_immediate_job(jobs[i]);
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
