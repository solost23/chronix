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
scheduler->add_once_job(std::chrono::system_clock::now() + std::chrono::seconds(3), []() { printer("[任务2]延时3秒执行"); });
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

| Round | Total Executions | Success Count | Failure Count | Avg Time (ms) | Max Time (ms) | Min Time (ms) | Total Time (s) | Throughput (tps) | Success Rate (%) | Error Rate (%) |
|-------|------------------|----------------|----------------|----------------|----------------|----------------|----------------|-------------------|------------------|----------------|
| 1     | 10000            | 9021           | 979            | 5.9849         | 12             | 1              | 1.02697        | 9737.42           | 90.21            | 9.79           |
| 2     | 20000            | 18007          | 1993           | 6.0179         | 21             | 1              | 2.04834        | 9763.98           | 90.035           | 9.965          |
| 3     | 30000            | 27013          | 2987           | 6.00807        | 14             | 1              | 3.06734        | 9780.45           | 90.0433          | 9.95667        |
| 4     | 40000            | 36043          | 3957           | 6.03862        | 20             | 1              | 4.10746        | 9738.37           | 90.1075          | 9.8925         |
| 5     | 50000            | 44909          | 5091           | 6.00676        | 15             | 1              | 5.10629        | 9791.84           | 89.818           | 10.182         |
| 6     | 60000            | 53899          | 6101           | 6.0068         | 16             | 1              | 6.12775        | 9791.53           | 89.8317          | 10.1683        |
| 7     | 70000            | 62887          | 7113           | 6.01863        | 14             | 1              | 7.16137        | 9774.66           | 89.8386          | 10.1614        |
| 8     | 80000            | 71933          | 8067           | 6.0443         | 20             | 1              | 8.22544        | 9725.92           | 89.9163          | 10.0838        |
| 9     | 90000            | 80960          | 9040           | 6.03264        | 17             | 1              | 9.20744        | 9774.71           | 89.9556          | 10.0444        |
| 10    | 100000           | 90118          | 9882           | 6.04804        | 19             | 1              | 10.2544        | 9751.95           | 90.118           | 9.882          |

---

## 📄 License

This project is open-source under the MIT License.
