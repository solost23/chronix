#include "example/example9.h"

#include "chronix/chronix.h"
#include "example/printer.h"

/*
 * immediate
 */

static const std::vector<std::function<void()>> jobs{
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

void example9()
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    try
    {
        scheduler->start();

        for (size_t i = 0; i != jobs.size(); i++)
        {
            scheduler->add_immediate_job(jobs[i]);
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
