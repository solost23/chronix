/*
 * performance.cpp
 */
#include <iostream> 
#include <fstream> 
#include <random> 

#include "chronix/define.h" 
#include "chronix/chronix.h" 

static const int THREAD_COUNT = std::thread::hardware_concurrency() * 4;; 

static const size_t ROUNDS = 10;
static const size_t JOBS = 5000; 
// 提交完任务等待时间，单位：秒
static const size_t JOBS_PER_ROUND = 30;

#define PERFORMANCE_ZH

static const std::string CSV_FILENAME_EN = "performance.csv";
static const std::string CSV_FILENAME_ZH = "性能测试.csv"; 
static const std::string CSV_HEADER_EN = "Round,Description,Total,Success,Error,Avg(ms),Max(ms),Min(ms),TotalTime(s),Throughput(tps),SuccessRate,ErrorRate";
static const std::string CSV_HEADER_ZH = "轮次编号,任务描述,总执行次数,成功次数,失败次数,平均耗时(ms),最大耗时(ms),最小耗时(ms),总耗时(s),吞吐量(tps),成功率,错误率"; 

int main(int argc, char* argv[])
{
    std::string filename = CSV_FILENAME_EN;
    std::string header = CSV_HEADER_EN;
#ifdef PERFORMANCE_ZH
    filename = CSV_FILENAME_ZH;
    header = CSV_HEADER_ZH;
#endif

    std::ofstream out(filename);
    out << header << "\r\n";

    std::vector<size_t> all_job_ids;

    auto scheduler = std::make_shared<ChronixScheduler>(THREAD_COUNT); 
    scheduler->set_metrics_enabled(true);

    for (size_t r = 0; r < ROUNDS; r ++)
    {
        std::vector<size_t> round_job_ids;

        for (size_t i = 0; i != JOBS; i ++)
        {
            std::string cron_expr = "*/" + std::to_string((i % 5) + 1) + " * * * * *";

            size_t job_id = scheduler->add_cron_job(cron_expr, [i]() {
                // 随机休眠 + 10%的概率失败
                using namespace std::chrono;
                static thread_local std::mt19937 rng(std::random_device{}());
                std::uniform_int_distribution<int> sleep_dist(1, 10);
                std::uniform_real_distribution<double> error_dist(0.0, 1.0);

                int sleep_time = sleep_dist(rng);
                std::this_thread::sleep_for(milliseconds(sleep_time));

                bool fail = error_dist(rng) < 0.1;

                if (fail) {
                    throw std::runtime_error("任务模拟错误");
                }
            }); 

            scheduler->start();

            round_job_ids.emplace_back(job_id);
        }

        all_job_ids.insert(all_job_ids.end(), round_job_ids.begin(), round_job_ids.end());

        std::cout << "[Round " << r + 1 << "] 周期任务已提交，总数：" << scheduler->get_job_count() << std::endl;

        auto r_start = std::chrono::steady_clock::now();
        std::this_thread::sleep_for(std::chrono::seconds(JOBS_PER_ROUND));
        auto r_end = std::chrono::steady_clock::now();
        double r_duration = std::chrono::duration<double>(r_end - r_start).count();

        for (const auto& job_id : round_job_ids)
        {
            JobMetrics metrics = scheduler->get_job_metrics(job_id); 

            if (metrics.execution_count == 0)
            {
                continue; 
            }

            out << r + 1 << "," << job_id << ","
                << metrics.execution_count << ","
                << metrics.success_count << ","
                << metrics.error_count << ","
                << metrics.average_duration().count() << ","
                << metrics.max_duration.count() << ","
                << (metrics.min_duration == std::chrono::milliseconds::max() ? 0 : metrics.min_duration.count()) << ","
                << metrics.total_duration.count() / 1000.0 << ","
                << (r_duration > 0 ? metrics.execution_count / r_duration : 0) << ","
                << metrics.success_rate() << ","
                << metrics.error_rate() << "\r\n";
        }

        out.flush(); 
    }

    std::cout << "✅ 所有轮次周期任务压测完成，指标写入成功" << std::endl; 
    return 0; 
}
