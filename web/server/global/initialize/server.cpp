#include "global/initialize/server.h"
#include "httplib/httplib.h"

void Run(std::function<std::unique_ptr<httplib::Server>(std::shared_ptr<Initialize>& initialize)> func, std::shared_ptr<Initialize>& initialize)
{
    auto server = func(initialize);
    if (!server)
    {
        std::cerr << "[Error] Failed to create server" << std::endl;
        return;
    }

    std::clog << "[Info] Server instance created. Starting in background..."
              << std::endl;

    std::thread server_thread([server = std::move(server), initialize = std::move(initialize)]() mutable {
        try
        {
            int port = initialize->get_config()->get_port();
            std::clog << "[Info] Server listening on 0.0.0.0:" << port << std::endl;
            server->listen("0.0.0.0", port);
        }
        catch (const std::exception& e)
        {
            std::cerr << "[Error] Server failed to start: " << e.what()
                      << std::endl;
        }
    });

    server_thread.detach();
}
