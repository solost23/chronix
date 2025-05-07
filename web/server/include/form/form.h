#pragma once

#include <chrono>
#include <string>

#include "nlohmann/json.hpp"

struct InsertCronTaskForm
{
    std::string cron;
    std::string callback_url;
};

inline void from_json(const nlohmann::json& j, InsertCronTaskForm& params)
{
    j.at("cron").get_to(params.cron);
    j.at("callback_url").get_to(params.callback_url);
}

struct InsertOnceTaskForm
{
    std::string run_at;
    std::string callback_url;
};

inline void from_json(const nlohmann::json& j, InsertOnceTaskForm& params)
{
    j.at("run_at").get_to(params.run_at);
    j.at("callback_url").get_to(params.callback_url);
}

struct InsertImmediateTaskForm
{
    std::string callback_url;
};

inline void from_json(const nlohmann::json& j, InsertImmediateTaskForm& params)
{
    j.at("callback_url").get_to(params.callback_url);
}

inline std::chrono::system_clock::time_point parse_iso_time(
    const std::string& iso_time)
{
    std::tm tm{};
    std::istringstream ss(iso_time);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

inline std::string force_http(std::string url)
{
    size_t pos = url.find("https://");
    if (pos != std::string::npos)
    {
        return url.replace(pos, 8, "http://");
    }
    return url;
}
