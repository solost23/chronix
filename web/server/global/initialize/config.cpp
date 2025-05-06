#include "global/initialize/config.h"

#include <string>

#include "config/config.h"
#include "global/global.h"

void init_config(const std::string& filepath)
{
    try
    {
        ServerConfig server_config(filepath);
    }
    catch (const std::exception& e)
    {
        std::runtime_error(std::string("init_config failed") + e.what());
    }
}
