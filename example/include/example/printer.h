#pragma once

#include <chrono>
#include <iomanip>
#include <iostream>

template <typename... Args>
void printer(Args &&...args)
{
    // 获取当前时间
    auto now = std::chrono::system_clock::now();
    auto now_time_t = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&now_time_t);

    std::cout << std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << " ";
    (std::cout << ... << std::forward<Args>(args)) << std::endl;
}
