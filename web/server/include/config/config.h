#pragma once

#include <fstream>
#include <string>

#include "yaml-cpp/yaml.h"

class ServerConfig
{
public:
    ServerConfig(const std::string& filepath){};

private:
    struct ChronixThreadPoolConfig
    {
        int min_threads;
        int max_threads;
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
