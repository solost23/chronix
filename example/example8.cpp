#include "example/example8.h"

#include "chronix/chronix.h"
#include "example/printer.h"

void example8()
{
    std::vector<
        std::pair<std::chrono::system_clock::time_point, std::function<void()>>>
        jobs{
            {std::chrono::system_clock::now() + std::chrono::seconds(1),
             []() { printer("[任务1]延时1秒执行"); }},
            {std::chrono::system_clock::now() + std::chrono::seconds(3),
             []() { printer("[任务2]延时3秒执行"); }},
            {std::chrono::system_clock::now() + std::chrono::seconds(5),
             []() { printer("[任务3]延时5秒执行"); }},
            {std::chrono::system_clock::now() + std::chrono::seconds(7),
             []() { printer("[任务4]延时7秒执行"); }},
            {std::chrono::system_clock::now() + std::chrono::seconds(9),
             []() { printer("[任务5]延时9秒执行"); }},
        };

    auto scheduler = std::make_shared<ChronixScheduler>(4);

    scheduler->start();

    for (size_t i = 0; i != jobs.size(); i++)
    {
        scheduler->add_once_job(jobs[i].first, jobs[i].second);
    }

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
