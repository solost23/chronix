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

---

## 🚀 Usage

### 1. Initialize the Scheduler

```cpp
// Initialize with a specified thread pool size (e.g., 4 worker threads)
auto scheduler = std::make_shared<ChronixScheduler>(4);
```

### 2. Add a Scheduled Task

```cpp
// Execute every 10 seconds
int job_id = scheduler.add_job("*/10 * * * * *", []() { std::cout << "Job executing" << std::endl; });

scheduler->set_start_callback([](int id) { std::cout << "Job " << id << " started" << std::endl; });
scheduler->set_success_callback([](int id) { std::cout << "Job " << id << " completed successfully" << std::endl; });
scheduler->set_error_callback([](int id, std::exception& e) { std::cerr << "Job " << id << " failed: " << e.what() << std::endl; });
// You can choose to persist the job state here
scheduler->set_end_callback([](int id) { std::cout << "Job " << id << " finished" << std::endl; });
```

### 3. Control Task State

```cpp
scheduler.pause_job(job_id);
scheduler.resume_job(job_id);
scheduler.remove_job(job_id);
```

### 4. Task Persistence

```cpp
scheduler->set_persistence(std::make_shared<DBPersistenceMySQL<Job>>("127.0.0.1", 33036, "root", "******", "chronix"));
scheduler.save_state();     // Save tasks
scheduler.load_state();     // Restore tasks

// Re-register task behaviors upon restore
scheduler.register_job_initializer(job_id, [](Job& job) {
    job.task = []() { std::cout << "Task re-bound to execution body" << std::endl; };
});
```

### 5. Start the Scheduler

```cpp
scheduler.start();
```
For more detailed usage, refer to the src/main.cpp file.

--- 

## 📄 License

This project is open-source under the MIT License.
