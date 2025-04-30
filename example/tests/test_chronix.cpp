#include <cassert>
#include <iostream>

#include "chronix/chronix.h"

void test_immediate_job();
void test_once_job();
void test_cron_job();

void test_delete_job();
void test_pause_resume_cron_job();
void test_delete_running_job_should_throw();
void test_pause_running_job_should_throw();

int main(int argc, char** argv)
{
    test_immediate_job();
    test_once_job();
    test_cron_job();

    test_delete_job();
    test_pause_resume_cron_job();
    test_delete_running_job_should_throw();
    test_pause_running_job_should_throw();

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

void test_delete_job()
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    bool run{false};

    try
    {
        scheduler->start();

        size_t job_id = scheduler->add_once_job(
            std::chrono::system_clock::now() + std::chrono::seconds(1),
            [&]() { run = true; });

        scheduler->remove_job(job_id);
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(2));
    scheduler->stop();
    assert(!run && "❌ Task ran!");
    std::cout << "✅ Task did not run!" << std::endl;
}

void test_pause_resume_cron_job()
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    std::atomic<size_t> run_count{0};

    try
    {
        scheduler->start();

        size_t job_id =
            scheduler->add_cron_job("*/1 * * * * *", [&] { run_count++; });

        std::this_thread::sleep_for(std::chrono::seconds(2));
        scheduler->pause_job(job_id);
        size_t run_count_after_pause = run_count;

        std::this_thread::sleep_for(std::chrono::seconds(2));
        assert(run_count_after_pause == run_count &&
               "Paused job should not run!");

        scheduler->resume_job(job_id);
        std::this_thread::sleep_for(std::chrono::seconds(2));
        assert(run_count > run_count_after_pause && "Resumed job should run!");
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    scheduler->stop();
    std::cout << "✅ Task ran!" << std::endl;
}

void test_delete_running_job_should_throw()
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    bool run{false};

    try
    {
        scheduler->start();
        size_t job_id = scheduler->add_immediate_job([&]() {
            run = true;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        assert(run && "❌ Task did not run!");

        scheduler->remove_job(job_id);
        assert(false && "Deleting a running job should throw");
    }
    catch (std::exception& e)
    {
        std::cout << "✅ Deleting a running job should throw" << std::endl;
    }

    scheduler->stop();
    std::cout << "✅ Task ran!" << std::endl;
}

void test_pause_running_job_should_throw()
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    bool run{false};

    try
    {
        scheduler->start();
        size_t job_id = scheduler->add_immediate_job([&]() {
            run = true;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        });

        std::this_thread::sleep_for(std::chrono::milliseconds(600));
        assert(run && "❌ Task did not run!");

        scheduler->pause_job(job_id);
        assert(false && "Pausing a running job should throw");
    }
    catch (std::exception& e)
    {
        std::cout << "✅ Pausing a running job should throw" << std::endl;
    }

    scheduler->stop();
    std::cout << "✅ Task ran!" << std::endl;
}