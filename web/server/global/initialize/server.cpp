#include "httplib/httplib.h"

void Run(std::function<std::unique_ptr<httplib::Server>()> func)
{
    auto server = func();
    if (!server)
    {
        std::cerr << "[Error] Failed to create server" << std::endl;
        return;
    }

    std::clog << "[Info] Server instance created. Starting in background..."
              << std::endl;

    std::thread server_thread([server = std::move(server)]() mutable {
        try
        {
            std::clog << "[Info] Server listening on 0.0.0.0:8080" << std::endl;
            server->listen("0.0.0.0", 8080);
        }
        catch (const std::exception& e)
        {
            std::cerr << "[Error] Server failed to start: " << e.what()
                      << std::endl;
        }
    });

    server_thread.detach();
}
