#include "global/initialize/main.h"

#include "global/initialize/config.h"

void initialize(const std::string& filepath)
{
    try
    {
        init_config(filepath);
        init_chronix();
    }
    catch (const std::exception& e)
    {
        std::runtime_error(std::string("initialize failed: ") + e.what());
    }
}
