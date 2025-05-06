#include "router/router.h"

#include <iostream>

#include "chronix/chronix.h"
#include "global/constant/code.h"
#include "global/initialize/initialize.h"
#include "global/initialize/server.h"

static const std::string WEB_CONFIG_PATH = "./web/server/config/config.yml";

void signaler(int sig);

std::atomic<bool> running(true);

int main(int argc, char* argv[])
{
    try
    {
        // init 
        auto initialize = std::make_shared<Initialize>(WEB_CONFIG_PATH);
        // run
        Run(Register, initialize); 
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << std::endl;
    }

    std::signal(SIGINT, signaler);
    std::signal(SIGTERM, signaler);
    while(running) {
        if (!running) break;
        std::this_thread::sleep_for(std::chrono::milliseconds(200));
    }

    return 0;
}

void signaler(int sig)
{
    running = false;

    std::ostringstream oss;
    oss << "Interrupt signal ("; oss << sig; oss << ") received";
    std::clog << "[Info] " << oss.str() << std::endl; 
}
