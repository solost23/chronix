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

| **Configuration Item**         | **Configuration Item**                            |
| -------------- | ------------------------------- |
| CPU Threads     | `std::thread::hardware_concurrency() * 4` |
| Tasks per Round     | 5,000 10,000 15,000 20,000 25,000 30,000 35,000 40,000, 45,000, 50,000 |
| Total Rounds         | 10                              |
| Total Tasks       | 275,000                            |
| Time per Round   | 30 seconds                          |
| Scheduling Frequency       | Every 1 ~ 5 seconds (cron expression */N * * * * *) |
| Task Body         | Random sleep time between 1 ~ 10 ms, 10% chance of failure           |

### 📈 Performance Test Results

| Round | Total Executions | Success Count | Failure Count | Avg Duration (ms) | Max Duration (ms) | Min Duration (ms) | Total Time (s) | Throughput (TPS) | Success Rate (%) | Error Rate (%) |
|-------|------------------|----------------|----------------|--------------------|--------------------|--------------------|----------------|-------------------|------------------|----------------|
| 1     | 55825            | 50346          | 5479           | 5.476              | 20                 | 1                  | 330.400        | 168.961864        | 90.1854          | 9.8146         |
| 2     | 57665            | 51889          | 5776           | 5.4994             | 18                 | 1                  | 340.963        | 169.123923        | 89.9835          | 10.0165        |
| 3     | 62483            | 56152          | 6331           | 5.5478             | 22                 | 1                  | 370.231        | 168.767607        | 89.8676          | 10.1324        |
| 4     | 67550            | 60772          | 6778           | 5.57895            | 21                 | 1                  | 399.779        | 168.968355        | 89.9660          | 10.0340        |
| 5     | 74159            | 66721          | 7438           | 5.59664            | 21                 | 1                  | 438.203        | 169.234350        | 89.9702          | 10.0298        |
| 6     | 76208            | 68501          | 7707           | 5.58863            | 25                 | 1                  | 447.875        | 170.154619        | 89.8869          | 10.1131        |
| 7     | 85365            | 76722          | 8643           | 5.64623            | 24                 | 1                  | 505.051        | 169.022534        | 89.8752          | 10.1248        |
| 8     | 82672            | 74137          | 8535           | 5.6996             | 30                 | 1                  | 489.611        | 168.852415        | 89.6761          | 10.3239        |
| 9     | 94120            | 84739          | 9381           | 5.67836            | 28                 | 1                  | 555.647        | 169.388119        | 90.0329          | 9.9671         |
| 10    | 95224            | 85976          | 9248           | 5.89842            | 33                 | 1                  | 573.867        | 165.933918        | 90.2882          | 9.7118         |

---

## 📄 License

This project is open-source under the MIT License.
