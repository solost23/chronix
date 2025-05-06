#include "global/initialize/chronix.h"

#include "chronix/chronix.h"
#include "global/global.h"

void init_chronix()
{
    try
    {
        scheduler = std::make_shared<ChronixScheduler>(
            server_config.get_min_threads(), server_config.get_max_threads());
        scheduler->start();
    }
    catch (const std::exception& e)
    {
        std::runtime_error(std::string("Error initializing Chronix: ") +
                           e.what());
    }
}
