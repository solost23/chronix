#pragma once

#include <string>

#include "chronix/chronix.h"
#include "config/config.h"

class Initialize
{
public:
    Initialize(const std::string& filepath);

    const std::shared_ptr<ServerConfig> get_config(); 
    const std::shared_ptr<ChronixScheduler> get_scheduler(); 
    
private:
    std::shared_ptr<ServerConfig> init_config(const std::string& filepath);
    std::shared_ptr<ChronixScheduler> init_chronix(const std::shared_ptr<ServerConfig>& server_config);

    std::shared_ptr<ServerConfig> server_config;
    std::shared_ptr<ChronixScheduler> scheduler;  
}; 
