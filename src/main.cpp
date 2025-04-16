/*
 * test program
 */

#include <iostream>
#include <memory> 
#include <utility>

#include "chronoix/chronoix.h"

void example1();
void example2();
void example3(); 
void example4(); 
void example5(); 

int main(int argc, char* argv[])
{
    std::thread t1(example1);
    std::thread t2(example2);
    std::thread t3(example3);
    std::thread t4(example4); 
    std::thread t5(example5); 
    if (t1.joinable())
    {
        t1.join();
    }
    if (t2.joinable())
    {
        t2.join();
    }
    if (t3.joinable())
    {
        t3.join();
    }
    if (t4.joinable())
    {
        t4.join(); 
    }
    if (t5.joinable())
    {
        t5.join(); 
    }
    
    return 0;  
}

/*
 * test second
 */
void example1()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"*/1 * * * * *", [](){ std::cout << "[任务1] 每1秒执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"*/3 * * * * *", [](){ std::cout << "[任务2] 每3秒执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"*/5 * * * * *", [](){ std::cout << "[任务3] 每5秒执行一次: " << std::time(nullptr) << std::endl; }},
        {"*/7 * * * * *", [](){ std::cout << "[任务4] 每7秒执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"*/9 * * * * *", [](){ std::cout << "[任务5] 每9秒执行一次: " << std::time(nullptr) << std::endl; }},  
    }; 

    auto scheduler = std::make_shared<ChronoixScheduler>(4); 

    for (int i = 0; i != jobs.size(); i ++)
    {
        scheduler->add_job(jobs[i].first, jobs[i].second);
    }

    scheduler->start(); 

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

/*
 * test minute
 */
void example2()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"0 */1 * * * *", [](){ std::cout << "[任务1] 每1分执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"0 */3 * * * *", [](){ std::cout << "[任务2] 每3分执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"0 */5 * * * *", [](){ std::cout << "[任务3] 每5分执行一次: " << std::time(nullptr) << std::endl; }},
        {"0 */7 * * * *", [](){ std::cout << "[任务4] 每7分执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"0 */9 * * * *", [](){ std::cout << "[任务5] 每9分执行一次: " << std::time(nullptr) << std::endl; }},  
    }; 

    auto scheduler = std::make_shared<ChronoixScheduler>(4); 

    for (int i = 0; i != jobs.size(); i ++)
    {
        scheduler->add_job(jobs[i].first, jobs[i].second);
    }

    scheduler->start();  

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }  
}

/*
 * test 
 */
void example3()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"0 15 0 * * *", [](){ std::cout << "[任务1] 每天00:15执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"0 16 0 * * *", [](){ std::cout << "[任务2] 每天00:16执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"0 17 0 * * *", [](){ std::cout << "[任务3] 每天00:17执行一次: " << std::time(nullptr) << std::endl; }},
        {"0 18 0 * * *", [](){ std::cout << "[任务4] 每天00:18执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"0 19 0 * * *", [](){ std::cout << "[任务5] 每天00:19执行一次: " << std::time(nullptr) << std::endl; }},  
    }; 

    auto scheduler = std::make_shared<ChronoixScheduler>(4); 

    for (int i = 0; i != jobs.size(); i ++)
    {
        scheduler->add_job(jobs[i].first, jobs[i].second);
    }

    scheduler->start();  

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }  
}

/*
 * test callback
 */
void example4()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"*/5 * * * * *", [](){ std::cout << "[任务1] 每5秒执行一次: " << std::time(nullptr) << std::endl; throw std::runtime_error("[任务1] 执行失败"); }}, 
        {"*/8 * * * * *", [](){ std::cout << "[任务2] 每8秒执行一次: " << std::time(nullptr) << std::endl; }}, 
    }; 

    auto scheduler = std::make_shared<ChronoixScheduler>(4);

    auto error_callback = [](int job_id, const std::exception& e) {
        std::cerr << "任务ID: " << job_id << " 执行失败，错误: " << e.what() << std::endl; 
    }; 
    auto success_callback = [](int job_id) {
        std::cout << "任务ID: " << job_id << " 执行成功" << std::endl; 
    };

    for (int i = 0; i != jobs.size(); i ++)
    {
        scheduler->add_job(jobs[i].first, jobs[i].second, error_callback, success_callback);
    }

    scheduler->start();

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5)); 
    }
}

/*
 * test persistence
 */
void example5()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"*/5 * * * * *", [](){ std::cout << "[任务1] 每5秒执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"*/10 * * * * *", [](){ std::cout << "[任务2] 每10秒执行一次: " << std::time(nullptr) << std::endl; }}, 
        {"*/20 * * * * *", [](){ std::cout << "[任务3] 每20秒执行一次: " << std::time(nullptr) << std::endl; }},
    };

    auto scheduler = std::make_shared<ChronoixScheduler>(4);
    scheduler->set_persistence(std::make_shared<FilePersistenceJson<Job>>("./jobs.json"));

    auto error_callback = [](int job_id, const std::exception& e) {
        std::cerr << "任务ID: " << job_id << " 执行失败，错误: " << e.what() << std::endl; 
    }; 
    auto success_callback = [](int job_id) {
        std::cout << "任务ID: " << job_id << " 执行成功" << std::endl; 
    };

    for (int i = 0; i != jobs.size(); i ++)
    {
        scheduler->add_job(jobs[i].first, jobs[i].second, error_callback, success_callback);
    }

    scheduler->save_state(); 

    // 注册缺失函数
    scheduler->register_job_initializer(1, [error_callback, success_callback](Job& job) {
        job.task = []() { std::cout << "[任务1] 每5秒执行一次: " << std::time(nullptr) << std::endl; };
        job.success_callback = success_callback; 
        job.error_callback = error_callback;
    }); 

    scheduler->register_job_initializer(2, [error_callback, success_callback](Job& job) {
        job.task = []() { std::cout << "[任务2] 每10秒执行一次: " << std::time(nullptr) << std::endl; };
        job.success_callback = success_callback; 
        job.error_callback = error_callback;
    }); 

    scheduler->register_job_initializer(3, [error_callback, success_callback](Job& job) {
        job.task = []() { std::cout << "[任务3] 每20秒执行一次: " << std::time(nullptr) << std::endl; };
        job.success_callback = success_callback; 
        job.error_callback = error_callback;
    }); 

    scheduler->load_state(); 
    scheduler->start(); 

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5)); 
    }
}
