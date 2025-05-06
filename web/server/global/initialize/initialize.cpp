#include "global/initialize/initialize.h"

Initialize::Initialize(const std::string& filepath)
{
    try 
    {
        server_config = init_config(filepath);
        scheduler = init_chronix(server_config);
    }
    catch (const std::exception& e)
    {
        std::runtime_error(std::string("initialize failed: ") + e.what());
    }
}

const std::shared_ptr<ServerConfig> Initialize::get_config()
{
    return server_config; 
}

const std::shared_ptr<ChronixScheduler> Initialize::get_scheduler()
{
    return scheduler; 
}

std::shared_ptr<ServerConfig> Initialize::init_config(const std::string& filepath)
{
    try 
    {
        auto server_config = std::make_shared<ServerConfig>(filepath); 
        
        return server_config; 
    }
    catch (const std::exception& e)
    {
        std::runtime_error(std::string("init_config failed") + e.what());
    }
    return nullptr; 
}

std::shared_ptr<ChronixScheduler> Initialize::init_chronix(const std::shared_ptr<ServerConfig>& server_config)
{
    try 
    {
        scheduler = std::make_shared<ChronixScheduler>(
            server_config->get_min_threads(), server_config->get_max_threads());
        scheduler->start();

        return scheduler;
    }
    catch (const std::exception& e)
    {
        std::runtime_error(std::string("Error initializing Chronix: ") +
                        e.what());
    }
    return nullptr; 
}
