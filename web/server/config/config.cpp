#include "config/config.h"

ServerConfig::ServerConfig(const std::string& filepath)
{
    std::ifstream fp(filepath);
    if (!fp)
    {
        throw std::runtime_error("Failed to open config file");
    }

    YAML::Node node = YAML::Load(fp);

    try
    {
        name = node["name"].as<std::string>();
        mode = node["mode"].as<std::string>();
        port = node["port"].as<int>();
        time_location = node["time_location"].as<std::string>();
        chronix_config.thread_pool_config.min_threads =
            node["thread_pool_config"]["min_threads"].as<size_t>();
        chronix_config.thread_pool_config.max_threads =
            node["thread_pool_config"]["max_threads"].as<size_t>();
    }
    catch (const YAML::Exception& e)
    {
        throw std::runtime_error(std::string("Failed to parse config file") +
                                 e.what());
    }
}

std::string ServerConfig::get_name() const
{
    return name;
}

std::string ServerConfig::get_mode() const
{
    return mode;
}

int ServerConfig::get_port() const
{
    return port;
}

size_t ServerConfig::get_min_threads() const
{
    return chronix_config.thread_pool_config.min_threads;
}

size_t ServerConfig::get_max_threads() const
{
    return chronix_config.thread_pool_config.max_threads;
}
