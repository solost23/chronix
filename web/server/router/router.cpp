#include "router/router.h"

#include "controller/controller.h"

std::unique_ptr<httplib::Server> Register(
    std::shared_ptr<Initialize>& initialize)
{
    auto server = std::make_unique<httplib::Server>();

    auto controller = std::make_shared<Controller>(initialize);

    server->Get(HEARTBEAT,
                [controller](const httplib::Request& req,
                             httplib::Response& resp) { success(resp, ""); });

    {
        server->Post(INSERT_CRON_TASK, [controller](const httplib::Request& req,
                                                    httplib::Response& resp) {
            controller->insert_cron_task(req, resp);
        });

        server->Post(INSERT_ONCE_TASK, [controller](const httplib::Request& req,
                                                    httplib::Response& resp) {
            controller->insert_once_task(req, resp);
        });

        server->Post(
            INSERT_IMMEDIATE_TASK,
            [controller](const httplib::Request& req, httplib::Response& resp) {
                controller->insert_immediate_task(req, resp);
            });
    }

    return server;
}
