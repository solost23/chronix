#pragma once

#include <chrono>
#include <regex>
#include <stdexcept>
#include <string>
#include <tuple>

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

struct SetCallbackForm
{
    size_t id;
    std::string callback_url;
};

inline void from_json(const nlohmann::json& j, SetCallbackForm& params)
{
    j.at("id").get_to(params.id);
    j.at("callback_url").get_to(params.callback_url);
}

struct SetMetricsEnabledForm
{
    bool enabled;
};

inline void from_json(const nlohmann::json& j, SetMetricsEnabledForm& params)
{
    j.at("enabled").get_to(params.enabled);
}

struct IDForm
{
    size_t id;
};

inline void from_json(const nlohmann::json& j, IDForm& params)
{
    j.at("id").get_to(params.id);
}

struct Metrics
{
    size_t execution_count;
    size_t success_count;
    size_t error_count;

    std::string last_run_time;
    int64_t last_duration;
    int64_t total_duration;

    int64_t max_duration;
    int64_t min_duration;

    std::vector<std::string> success_times;
    std::vector<std::string> error_times;

    std::string description;

    int64_t average_duration;
    double success_rate;
    double error_rate;
};

inline void to_json(nlohmann::json& j, const Metrics& metrics)
{
    j = nlohmann::json{{"execution_count", metrics.execution_count},
                       {"success_count", metrics.success_count},
                       {"error_count", metrics.error_count},
                       {"last_run_time", metrics.last_run_time},
                       {"last_duration", metrics.last_duration},
                       {"total_duration", metrics.total_duration},
                       {"max_duration", metrics.max_duration},
                       {"min_duration", metrics.min_duration},
                       {"success_times", metrics.success_times},
                       {"error_times", metrics.error_times},
                       {"description", metrics.description},
                       {"average_duration", metrics.average_duration},
                       {"success_rate", metrics.success_rate},
                       {"error_rate", metrics.error_rate}};
}

inline void from_json(const nlohmann::json& j, Metrics& metrics)
{
    j.at("execution_count").get_to(metrics.execution_count);
    j.at("success_count").get_to(metrics.success_count);
    j.at("error_count").get_to(metrics.error_count);
    j.at("last_run_time").get_to(metrics.last_run_time);
    j.at("last_duration").get_to(metrics.last_duration);
    j.at("total_duration").get_to(metrics.total_duration);
    j.at("max_duration").get_to(metrics.max_duration);
    j.at("min_duration").get_to(metrics.min_duration);
    j.at("success_times").get_to(metrics.success_times);
    j.at("error_times").get_to(metrics.error_times);
    j.at("description").get_to(metrics.description);
    j.at("average_duration").get_to(metrics.average_duration);
    j.at("success_rate").get_to(metrics.success_rate);
    j.at("error_rate").get_to(metrics.error_rate);
}

inline std::chrono::system_clock::time_point parse_iso_time(
    const std::string& iso_time)
{
    std::tm tm{};
    std::istringstream ss(iso_time);
    ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return std::chrono::system_clock::from_time_t(std::mktime(&tm));
}

inline std::string to_iso_time(const std::chrono::system_clock::time_point& tp)
{
    std::time_t time = std::chrono::system_clock::to_time_t(tp);
    std::tm tm = *std::localtime(&time);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%dT%H:%M:%S");
    return oss.str();
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

inline std::pair<std::string, std::string> extract_host_and_path(
    const std::string& url)
{
    // 改进的正则表达式：支持 IPv6 和更复杂的路径结构
    std::regex url_regex(
        R"(^(https?)://(\[[^\]]+\]|[^/\:?]+)(:\d+)?(/[^?#]*)?(\?.*)?$)",
        std::regex::icase);
    std::smatch match;

    if (std::regex_match(url, match, url_regex))
    {
        std::string host = match[2].str();   // 域名或 IP（包含IPv6）
        std::string port = match[3].str();   // 端口部分，如 ":8080"
        std::string path = match[4].str();   // 路径部分
        std::string query = match[5].str();  // 参数部分

        // 如果路径为空，则设为根路径 "/"
        if (path.empty())
            path = "/";

        path = path + query;
        return {host + port, path};
    }

    throw std::invalid_argument("Invalid URL: " + url);
}
