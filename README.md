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
| Tasks per Round     | 5000                            |
| Total Rounds         | 10                              |
| Total Tasks       | 50,000                           |
| Time per Round   | 30 seconds                          |
| Scheduling Frequency       | Every 1 ~ 5 seconds (cron expression */N * * * * *) |
| Task Body         | Random sleep time between 1 ~ 10 ms, 10% chance of failure           |

### 📈 Performance Test Results

| Round | Total Executions | Success Count | Failure Count | Avg Time (ms) | Max Time (ms) | Min Time (ms) | Total Time (s) | Throughput (tps)    | Success Rate (%)     | Error Rate (%)       |
|-------|------------------|---------------|----------------|----------------|----------------|----------------|----------------|----------------------|-----------------------|------------------------|
| 1     | 59039            | 53037         | 6002           | 5.411          | 23             | 1              | 346.237         | 170.5161493427912     | 89.8338386490286      | 10.166161350971393     |
| 2     | 28845            | 25894         | 2951           | 5.4366         | 17             | 1              | 168.659         | 171.02556045037622    | 89.76945744496446     | 10.230542555035536     |
| 3     | 17597            | 15843         | 1754           | 5.5            | 16             | 1              | 103.127         | 170.63426648695295    | 90.03239188498038     | 9.967608115019605      |
| 4     | 12463            | 11189         | 1274           | 5.5408         | 16             | 1              | 72.697          | 171.43761090553943    | 89.77774211666532     | 10.22225788333467      |
| 5     | 10000            | 8976          | 1024           | 5.584          | 17             | 1              | 58.284          | 171.57367373550204    | 89.76                 | 10.24                  |
| 6     | 8806             | 7930          | 876            | 5.7072         | 15             | 1              | 51.91           | 169.63976112502408    | 90.05223711106063     | 9.94776288893936       |
| 7     | 5000             | 4477          | 523            | 5.7642         | 14             | 1              | 28.821          | 173.48461191492316    | 89.54                 | 10.46                  |
| 8     | 5000             | 4495          | 505            | 5.9122         | 19             | 1              | 29.561          | 169.14177463549947    | 89.9                  | 10.100000000000001     |
| 9     | 5000             | 4498          | 502            | 5.852          | 16             | 1              | 29.26           | 170.88174982911826    | 89.96                 | 10.040000000000001     |
| 10    | 5000             | 4481          | 519            | 5.8808         | 17             | 1              | 29.404          | 170.04489185144877    | 89.62                 | 10.38                  |

---

## 📄 License

This project is open-source under the MIT License.
