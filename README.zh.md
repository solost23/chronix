# Chronix Scheduler 

[English](./README.md) | [简体中文](./README.zh.md)

---

## 🔧 项目概览

轻量级、高性能的 C++ 定时任务调度器，支持 Cron 表达式调度、任务状态持久化、钩子回调、线程池并发执行，适用于服务端定时任务调度场景。

---

## ✨ 功能特色

- ⏱️ **Cron 表达式调度**：支持秒级精度
- 🧵 **线程池并发执行**：任务并发调度，性能强悍
- 🧩 **任务钩子机制**：支持开始、成功、结束、失败回调
- 🔄 **任务持久化**：任务状态可保存与恢复
- ⏯️ **任务控制**：支持添加、暂停、恢复、删除任务
- ⏳ **延时一次性任务**：支持延时执行一次性任务，秒级精度
---

## 🚀 使用方式

### 1. 初始化调度器

```cpp
// 初始化时指定线程池大小（例如 4 个工作线程）
auto scheduler = std::make_shared<ChronixScheduler>(4);
```

### 2. 添加定时任务

```cpp
// 每 10 秒执行一次
int job_id = scheduler.add_cron_job("*/10 * * * * *", []() { std::cout << "任务执行" << std::endl; });

scheduler->set_start_callback(job_id, [](int id) { std::cout << "任务 " << id << " 开始执行" << std::endl; });
scheduler->set_success_callback(job_id, [](int id) { std::cout << "任务 " << id << " 执行成功" << std::endl; });
scheduler->set_error_callback(job_id, [](int id, std::exception& e) { std::cerr << "任务 " << id << " 执行失败: " << e.what() << std::endl; });
// 这里可选择持久化任务状态
scheduler->set_end_callback(job_id, [](int id) { std::cout << "任务" << id << " 执行结束" << std::endl; });
```

### 3. 添加延时任务

```cpp
// 延时三秒后执行一次
scheduler->add_one_time_job(std::chrono::system_clock::now() + std::chrono::seconds(3), []() { printer("[任务2]延时3秒执行"); });
```

### 4. 控制任务状态

```cpp
scheduler.pause_job(job_id);
scheduler.resume_job(job_id);
scheduler.remove_job(job_id);
```

### 5. 任务持久化

```cpp
scheduler->set_persistence(std::make_shared<DBPersistenceMySQL<Job>>("127.0.0.1", 33036, "root", "******", "chronix"));
scheduler.save_state();     // 保存任务
scheduler.load_state();     // 恢复任务

// 恢复时需重新注册任务行为
scheduler.register_job_initializer(job_id, [](Job& job) {
    job.task = []() { std::cout << "任务重新绑定执行体" << std::endl; };
});
```

### 6. 启动调度器

```cpp
scheduler.start();
```
更详细的使用案例可查看 src/main.cpp 文件。

---

## 📄 许可协议

本项目基于 MIT License 开源使用。
