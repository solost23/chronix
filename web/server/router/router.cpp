#include "router/router.h"

std::unique_ptr<httplib::Server> Register(std::shared_ptr<Initialize>& initialize)
{
    auto server = std::make_unique<httplib::Server>();
    server->Get("/hi", [](const httplib::Request& req, httplib::Response& res) {
        res.set_content("Hello World!", "text/plain");
    });

    return server;
}
