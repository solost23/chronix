#include <cassert>
#include <iostream>

#include "chronix/chronix.h"

void test_immediate_job();
void test_once_job();
void test_cron_job();

int main(int argc, char** argv)
{
    test_immediate_job();
    test_once_job();
    test_cron_job();

    std::cout << "✅ All tests passed!" << std::endl;
    return 0;
}

void test_immediate_job()
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    bool run{false};
    try
    {
        scheduler->start();

        scheduler->add_immediate_job([&]() { run = true; });
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(550));
    scheduler->stop();

    assert(run && "❌ Task did not run!");
    std::cout << "✅ Task ran!" << std::endl;
}

void test_once_job()
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    bool run{false};
    try
    {
        scheduler->start();

        scheduler->add_once_job(
            std::chrono::system_clock::now() + std::chrono::milliseconds(100),
            [&]() { run = true; });
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(650));
    scheduler->stop();

    assert(run && "❌ Task did not run!");
    std::cout << "✅ Task ran!" << std::endl;
}

void test_cron_job()
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    std::atomic<size_t> run_count{0};

    try
    {
        scheduler->start();
        scheduler->add_cron_job("*/1 * * * * *", [&]() { run_count++; });
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::milliseconds(5550));
    scheduler->stop();

    assert(run_count >= 5 && "❌ Task did not run!");
    std::cout << "✅ Task ran!" << std::endl;
}
