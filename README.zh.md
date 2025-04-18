# Chronix Scheduler 

[English](./README.md) | [简体中文](./README.zh.md)

---

## 🔧 项目概览

轻量级、高性能的 C++ 定时任务调度器，支持 Cron 表达式调度、任务状态持久化、钩子回调、线程池并发执行，适用于服务端定时任务调度场景。

---

## ✨ 功能特色

- ⏱️ **Cron 表达式调度**：支持秒级精度
- 🧵 **线程池并发执行**：任务并发调度，性能强悍
- 🧩 **任务钩子机制**：支持开始、成功、失败回调
- 🔄 **任务持久化**：任务状态可保存与恢复
- ⏯️ **任务控制**：支持添加、暂停、恢复、删除任务

---

## 🚀 使用方式

### 1. 初始化调度器

```cpp
// 初始化时指定线程池大小（例如 4 个工作线程）
auto scheduler = std::make_shared<ChronixScheduler>(4);
```

### 2. 添加定时任务

```cpp
int job_id = scheduler.add_job(
    "*/10 * * * * *",  // 每 10 秒执行
    []() {
        std::cout << "任务执行" << std::endl;
    },
    [](int id, const std::exception& e) {
        std::cerr << "任务 " << id << " 出错: " << e.what() << std::endl;
    },
    [](int id) {
        std::cout << "任务 " << id << " 成功" << std::endl;
    },
    [](int id) {
        std::cout << "任务 " << id << " 开始执行" << std::endl;
    }
);
```

### 3. 控制任务状态

```cpp
scheduler.pause_job(job_id);
scheduler.resume_job(job_id);
scheduler.remove_job(job_id);
```

### 4. 任务持久化

```cpp
scheduler->set_persistence(std::make_shared<DBPersistenceMySQL<Job>>("127.0.0.1", 33036, "root", "******", "chronix"));
scheduler.save_state();     // 保存任务
scheduler.load_state();     // 恢复任务

// 恢复时需重新注册任务行为
scheduler.register_job_initializer(job_id, [](Job& job) {
    job.task = []() { std::cout << "任务重新绑定执行体" << std::endl; };
});
```

### 5. 启动调度器

```cpp
scheduler.start();
```
更详细的使用案例可查看 /src/main.cpp 文件。

---

## 📄 许可协议

本项目基于 MIT License 开源使用。
