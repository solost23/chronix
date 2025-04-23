# Chronix Scheduler 

[English](./README.md) | [简体中文](./README.zh.md)

---

## 🔧 Project Overview

Chronix is a lightweight, high-performance C++ task scheduler that supports Cron expression scheduling, task state persistence, hook callbacks, and thread pool concurrency. It is suitable for server-side scheduled task management.

---

## ✨ Features

- ⏱️ **Cron Expression Scheduling**: Supports second-level precision
- 🧵 **Thread Pool Concurrency**: Concurrent task scheduling with high performance
- 🧩 **Task Hook Mechanism**: Supports start, success, end, and failure callbacks
- 🔄 **Task Persistence**: Task state can be saved and restored
- ⏯️ **Task Control**: Supports adding, pausing, resuming, and removing tasks
- ⏳ **Delayed One-Time Jobs**: Supports delayed execution of one-time jobs with second-level precision
- 📊 **Job Metrics Tracking**: Supports runtime metrics including execution count, success/failure count, and execution time; can be integrated with Prometheus for real-time monitoring

---

## 🚀 Usage

### 1. Initialize the Scheduler

```cpp
// Initialize with a specified thread pool size (e.g., 4 worker threads)
auto scheduler = std::make_shared<ChronixScheduler>(4);
```

### 2. Add a Scheduled Job

```cpp
// Execute every 10 seconds
int job_id = scheduler.add_cron_job("*/10 * * * * *", []() { std::cout << "Job executing" << std::endl; });

scheduler->set_start_callback(job_id, [](int id) { std::cout << "Job " << id << " started" << std::endl; });
scheduler->set_success_callback(job_id, [](int id) { std::cout << "Job " << id << " completed successfully" << std::endl; });
scheduler->set_error_callback(job_id, [](int id, std::exception& e) { std::cerr << "Job " << id << " failed: " << e.what() << std::endl; });
// You can choose to persist the job state here
scheduler->set_end_callback(job_id, [](int id) { std::cout << "Job " << id << " finished" << std::endl; });
```

### 3. Add A Delayed Job

```cpp
// Execute once after a delay of three seconds
scheduler->add_one_time_job(std::chrono::system_clock::now() + std::chrono::seconds(3), []() { printer("[任务2]延时3秒执行"); });
```

### 4. Control Job State

```cpp
scheduler.pause_job(job_id);
scheduler.resume_job(job_id);
scheduler.remove_job(job_id);
```

### 5. Job Persistence

```cpp
scheduler->set_persistence(std::make_shared<DBPersistenceMySQL<Job>>("127.0.0.1", 33036, "root", "******", "chronix"));
scheduler.save_state();     // Save tasks
scheduler.load_state();     // Restore tasks

// Re-register task behaviors upon restore
scheduler.register_job_initializer(job_id, [](Job& job) {
    job.task = []() { std::cout << "Task re-bound to execution body" << std::endl; };
});
```

### 6. Start the Scheduler

```cpp
scheduler.start();
```
For more detailed usage, refer to the example/example.cpp file.

---

## 📊 Performance Test Report

### 🧪 Performance Test Configuration

| **Configuration Item** | **Parameter**                               |
| ---------------------- | ------------------------------------------- |
| CPU Threads            | `std::thread::hardware_concurrency() * 8`   |
| Task Step per Round    | 10000                                      |
| Total Rounds           | 10                                          |
| Task Body              | Random delay of 1 ~ 10 milliseconds, 10% failure probability |

### 📈 Performance Test Results

| Round | Total Executions | Successes | Failures | Avg Duration (ms) | Max Duration (ms) | Min Duration (ms) | Total Time (s) | Throughput (tps) | Success Rate (%) | Error Rate (%) |
|-------|------------------|-----------|----------|--------------------|--------------------|--------------------|----------------|-------------------|------------------|----------------|
| 1     | 10000            | 8998      | 1002     | 5.9957             | 23                 | 1                  | 4.13901        | 2416.04           | 89.98            | 10.02           |
| 2     | 20000            | 17983     | 2017     | 6.04395            | 23                 | 1                  | 7.40635        | 2700.39           | 89.915           | 10.085          |
| 3     | 30000            | 26935     | 3065     | 5.9855             | 24                 | 1                  | 12.5121        | 2397.69           | 89.7833          | 10.2167         |
| 4     | 40000            | 36101     | 3899     | 5.9654             | 18                 | 1                  | 16.9638        | 2357.97           | 90.2525          | 9.7475          |
| 5     | 50000            | 44929     | 5071     | 5.98532            | 21                 | 1                  | 21.2588        | 2351.96           | 89.858           | 10.142          |
| 6     | 60000            | 53933     | 6067     | 5.98988            | 17                 | 1                  | 25.4099        | 2361.28           | 89.8883          | 10.1117         |
| 7     | 70000            | 63050     | 6950     | 6.12439            | 22                 | 1                  | 30.4221        | 2300.96           | 90.0714          | 9.92857         |
| 8     | 80000            | 72141     | 7859     | 5.9892             | 23                 | 1                  | 34.1778        | 2340.7            | 90.1762          | 9.82375         |
| 9     | 90000            | 80977     | 9023     | 6.02258            | 20                 | 1                  | 38.5402        | 2335.22           | 89.9744          | 10.0256         |
| 10    | 100000           | 89949     | 10051    | 6.0317             | 27                 | 1                  | 43.2521        | 2312.03           | 89.949           | 10.051          |

---

## 📄 License

This project is open-source under the MIT License.
