#pragma once

#include <fstream>
#include <string>

#include "yaml-cpp/yaml.h"

class ServerConfig
{
public:
    ServerConfig(const std::string& filepath);

    std::string get_name() const;
    std::string get_mode() const;
    int get_port() const;
    size_t get_min_threads() const;
    size_t get_max_threads() const;

private:
    struct ChronixThreadPoolConfig
    {
        size_t min_threads;
        size_t max_threads;
    };

    struct ChronixConfig
    {
        ChronixThreadPoolConfig thread_pool_config;
    };

    std::string name;
    std::string mode;
    int port;
    std::string time_location;
    ChronixConfig chronix_config;
};
