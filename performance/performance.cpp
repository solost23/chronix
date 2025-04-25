/*
 * performance.cpp
 */
#include <fstream>
#include <iostream>
#include <random>

#include "chronix/chronix.h"
#include "chronix/define.h"

static const int THREAD_COUNT = std::thread::hardware_concurrency() * 8;

static const size_t ROUNDS = 10;
static const size_t JOB_STEP = 10000;
// 提交完任务等待时间，单位：秒
// static const size_t JOBS_PER_ROUND = 30;

#define PERFORMANCE_ZH

static const std::string CSV_FILENAME_EN = "performance.csv";
static const std::string CSV_FILENAME_ZH = "性能测试.csv";
static const std::string CSV_HEADER_EN =
    "Round,Total,Success,Error,Avg(ms),Max(ms),Min(ms),TotalTime(s),Throughput("
    "tps),SuccessRate,ErrorRate";
static const std::string CSV_HEADER_ZH =
    "轮次编号,总执行次数,成功次数,失败次数,平均耗时(ms),最大耗时(ms),最小耗时("
    "ms),总耗时(s),吞吐量(tps),成功率,错误率";

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

    auto scheduler = std::make_shared<ChronixScheduler>(THREAD_COUNT);
    scheduler->set_metrics_enabled(true);

    scheduler->start();

    for (size_t r = 0; r < ROUNDS; ++r)
    {
        std::atomic<size_t> success_count{0};
        std::atomic<size_t> error_count{0};
        std::mutex mtx;
        std::condition_variable cv;
        std::atomic<size_t> done_count{0};

        std::vector<std::chrono::milliseconds> durations;
        std::chrono::milliseconds max_duration{0};
        std::chrono::milliseconds min_duration{
            std::chrono::milliseconds::max()};
        std::chrono::milliseconds total_duration{0};

        auto r_start = std::chrono::steady_clock::now();

        auto jobs = (r + 1) * JOB_STEP;
        for (size_t i = 0; i < jobs; ++i)
        {
            scheduler->add_once_job(
                std::chrono::system_clock::now() +
                    std::chrono::milliseconds(10),
                [&, i]() {
                    using namespace std::chrono;
                    auto start = steady_clock::now();

                    static thread_local std::mt19937 rng(
                        std::random_device{}());
                    std::uniform_int_distribution<int> sleep_dist(1, 10);
                    std::uniform_real_distribution<double> error_dist(0.0, 1.0);

                    int sleep_time = sleep_dist(rng);
                    std::this_thread::sleep_for(milliseconds(sleep_time));

                    auto end = steady_clock::now();
                    auto duration = duration_cast<milliseconds>(end - start);

                    {
                        std::lock_guard<std::mutex> lock(mtx);
                        durations.push_back(duration);
                        total_duration += duration;
                        if (duration > max_duration)
                            max_duration = duration;
                        if (duration < min_duration)
                            min_duration = duration;
                    }

                    if (error_dist(rng) < 0.1)
                    {
                        error_count++;
                    }
                    else
                    {
                        success_count++;
                    }

                    if (++done_count == jobs)
                    {
                        cv.notify_one();
                    }
                });
        }

        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [&]() { return done_count == jobs; });
        }

        auto r_end = std::chrono::steady_clock::now();
        double r_duration =
            std::chrono::duration<double>(r_end - r_start).count();

        double avg_ms = durations.empty()
                            ? 0
                            : total_duration.count() * 1.0 / durations.size();
        double tps = durations.size() / r_duration;
        double succ_rate = 100.0 * success_count / jobs;
        double err_rate = 100.0 * error_count / jobs;

        out << r + 1 << "," << jobs << "," << success_count << ","
            << error_count << "," << avg_ms << "," << max_duration.count()
            << ","
            << (min_duration == std::chrono::milliseconds::max()
                    ? 0
                    : min_duration.count())
            << "," << r_duration << "," << tps << "," << succ_rate << ","
            << err_rate << "\r\n";

        out.flush();
        std::cout << "[Round " << r + 1 << "] 一次性任务压测完成 ✅"
                  << std::endl;
    }

    std::cout << "✅ 所有轮次一次性任务压测完成，结果写入成功" << std::endl;
    return 0;
}
