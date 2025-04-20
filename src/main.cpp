/*
 * test program
 */

#include <iostream>
#include <memory> 
#include <utility>
#include <vector> 

#include "chronix/chronix.h"

template<typename... Args>
void printer(Args&&...); 

void example1();
void example2();
void example3(); 
void example4(); 
void example5(); 
void example6(); 
void example7(); 
void example8();

int main(int argc, char* argv[])
{
    std::vector<std::thread> examples{};

    examples.emplace_back(example1); 
    examples.emplace_back(example2);  
    examples.emplace_back(example3);  
    examples.emplace_back(example4);  
    examples.emplace_back(example5);  
    examples.emplace_back(example6); 
    examples.emplace_back(example7); 
    examples.emplace_back(example8);

    for (auto& example : examples)
    {
       if (example.joinable())
       {
           example.join();
       } 
    }
    
    return 0;
}

template<typename... Args>
void printer(Args&&... args)
{
    // 获取当前时间
    auto now = std::chrono::system_clock::now(); 
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_time_t);

    std::cout << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << " ";
    (std::cout << ... << std::forward<Args>(args)) << std::endl; 
}

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

/*
 * test 
 */
void example3()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"0 15 0 * * *", [](){ printer("[任务1] 每天00:15执行一次"); }}, 
        {"0 16 0 * * *", [](){ printer("[任务2] 每天00:16执行一次"); }}, 
        {"0 17 0 * * *", [](){ printer("[任务3] 每天00:17执行一次"); }},
        {"0 18 0 * * *", [](){ printer("[任务4] 每天00:18执行一次"); }}, 
        {"0 19 0 * * *", [](){ printer("[任务5] 每天00:19执行一次"); }},  
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

/*
 * test callback
 */
void example4()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"*/5 * * * * *", [](){ printer("[任务1] 每5秒执行一次"); throw std::runtime_error("[任务1] 执行失败"); }}, 
        {"*/8 * * * * *", [](){ printer("[任务2] 每8秒执行一次"); }}, 
    }; 

    auto scheduler = std::make_shared<ChronixScheduler>(4);

    auto start_callback = [](int job_id) {
        printer("任务ID: ", job_id, " 开始执行");
    }; 

    auto error_callback = [](int job_id, const std::exception& e) {
        printer("任务ID: ", job_id, " 执行失败，错误: ", e.what());
    }; 
    auto success_callback = [](int job_id) {
        printer("任务ID: ", job_id, " 执行成功");
    };

    for (size_t i = 0; i != jobs.size(); i ++)
    {
        int job_id = scheduler->add_cron_job(jobs[i].first, jobs[i].second);
        scheduler->set_start_callback(job_id, start_callback);
        scheduler->set_success_callback(job_id, success_callback);
        scheduler->set_error_callback(job_id, error_callback);
    }

    scheduler->start();

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5)); 
    }
}

/*
 * test file persistence json
 */
void example5()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"*/5 * * * * *", [](){ printer("[任务1] 每5秒执行一次"); }}, 
        {"*/10 * * * * *", [](){ printer("[任务2] 每10秒执行一次"); }}, 
        {"*/20 * * * * *", [](){ printer("[任务3] 每20秒执行一次"); }},
    };

    auto scheduler = std::make_shared<ChronixScheduler>(4);
    scheduler->set_persistence(std::make_shared<FilePersistenceJson<Job>>("./jobs.json"));

    auto error_callback = [](int job_id, const std::exception& e) { 
        printer("任务ID: ", job_id, " 执行失败，错误: ", e.what());
    }; 
    auto success_callback = [](int job_id) {
        printer("任务ID: ", job_id, " 执行成功");
    };

    for (size_t i = 0; i != jobs.size(); i ++)
    {
        int job_id = scheduler->add_cron_job(jobs[i].first, jobs[i].second);
        scheduler->set_success_callback(job_id, success_callback);
        scheduler->set_error_callback(job_id, error_callback);
    }

    scheduler->save_state(); 

    // 注册缺失函数
    scheduler->register_job_initializer(1, [error_callback, success_callback](Job& job) {
        job.task = []() { printer("[任务1] 每5秒执行一次"); };
        job.success_callback = success_callback; 
        job.error_callback = error_callback;
    }); 

    scheduler->register_job_initializer(2, [error_callback, success_callback](Job& job) {
        job.task = []() { printer("[任务2] 每10秒执行一次"); };
        job.success_callback = success_callback; 
        job.error_callback = error_callback;
    }); 

    scheduler->register_job_initializer(3, [error_callback, success_callback](Job& job) {
        job.task = []() { printer("[任务3] 每20秒执行一次"); };
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

/*
 * test db persistence mysql
 */

void example6()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"*/5 * * * * *", [](){ printer("[任务1] 每5秒执行一次"); }}, 
        {"*/10 * * * * *", [](){ printer("[任务2] 每10秒执行一次"); }}, 
        {"*/20 * * * * *", [](){ printer("[任务3] 每20秒执行一次"); }},
    };

    auto scheduler = std::make_shared<ChronixScheduler>(4);
    scheduler->set_persistence(std::make_shared<DBPersistenceMySQL<Job>>("127.0.0.1", 33036, "root", "123", "chronix"));

    auto error_callback = [](int job_id, const std::exception& e) {
        printer("任务ID: ", job_id, " 执行失败，错误: ", e.what());
    }; 
    auto success_callback = [](int job_id) {
        printer("任务ID: ", job_id, " 执行成功");
    };

    for (size_t i = 0; i != jobs.size(); i ++)
    {
        int job_id = scheduler->add_cron_job(jobs[i].first, jobs[i].second);
        scheduler->set_success_callback(job_id, success_callback);
        scheduler->set_error_callback(job_id, error_callback);
    }

    scheduler->save_state();

    // 注册缺失函数
    scheduler->register_job_initializer(1, [error_callback, success_callback](Job& job) {
        job.task = []() { printer("[任务1] 每5秒执行一次"); };
        job.success_callback = success_callback; 
        job.error_callback = error_callback;
    }); 

    scheduler->register_job_initializer(2, [error_callback, success_callback](Job& job) {
        job.task = []() { printer("[任务2] 每10秒执行一次"); };
        job.success_callback = success_callback; 
        job.error_callback = error_callback;
    }); 

    scheduler->register_job_initializer(3, [error_callback, success_callback](Job& job) {
        job.task = []() { printer("[任务3] 每20秒执行一次"); };
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

/*
 * test task status and result 
 */
void example7()
{
    std::vector<std::pair<std::string, std::function<void()>>> jobs{
        {"*/3 * * * * *", []() {printer("[任务1] 每3秒执行一次"); }}, // success
        {"*/5 * * * * *", []() {printer("[任务2] 每5秒执行一次"); throw std::runtime_error("[任务2] 执行失败"); }}, // failed
        {"*/7 * * * * *", []() {printer("[任务3] 每7秒执行一次"); }}, // paused
    }; 

    auto scheduler = std::make_shared<ChronixScheduler>(4);
    // scheduler->set_persistence(std::make_shared<FilePersistenceJson<Job>>("./jobs.json"));
    scheduler->set_persistence(std::make_shared<DBPersistenceMySQL<Job>>("127.0.0.1", 33036, "root", "123", "chronix"));

    // TODO: 任务结束钩子，持久化快照, 暂时全量更新
    auto end_callback = [scheduler](int job_id) {
        scheduler->save_state();
        printer("任务[", job_id, "] 保存成功"); 
    };

    for (size_t i = 0; i != jobs.size(); i ++)
    {
        int job_id = scheduler->add_cron_job(jobs[i].first, jobs[i].second);
        scheduler->set_end_callback(job_id, end_callback);
    }

    // 开始执行
    scheduler->start(); 

    std::this_thread::sleep_for(std::chrono::seconds(3));

    // 第三个任务暂停，暂停后手动保存
    scheduler->pause_job(3);
    // 保存结果到文件中
    scheduler->save_state();

    while(true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5)); 
    }
}

void example8()
{
    std::vector<std::pair<std::chrono::system_clock::time_point, std::function<void()>>> jobs{
        {std::chrono::system_clock::now() + std::chrono::seconds(1), []() { printer("[任务1]延时1秒执行"); }}, 
        {std::chrono::system_clock::now() + std::chrono::seconds(3), []() { printer("[任务2]延时3秒执行"); }}, 
        {std::chrono::system_clock::now() + std::chrono::seconds(5), []() { printer("[任务3]延时5秒执行"); }}, 
        {std::chrono::system_clock::now() + std::chrono::seconds(7), []() { printer("[任务4]延时7秒执行"); }}, 
        {std::chrono::system_clock::now() + std::chrono::seconds(9), []() { printer("[任务5]延时9秒执行"); }}, 
    }; 

    auto scheduler = std::make_shared<ChronixScheduler>(4);

    for (size_t i = 0; i != jobs.size(); i ++)
    {
        scheduler->add_one_time_job(jobs[i].first, jobs[i].second);
    }

    scheduler->start();

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}
