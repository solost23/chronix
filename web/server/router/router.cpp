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

        server->Post(
            SET_START_CALLBACK,
            [controller](const httplib::Request& req, httplib::Response& resp) {
                controller->set_start_callback(req, resp);
            });

        server->Post(
            SET_SUCCESS_CALLBACK,
            [controller](const httplib::Request& req, httplib::Response& resp) {
                controller->set_success_callback(req, resp);
            });

        server->Post(
            SET_ERROR_CALLBACK,
            [controller](const httplib::Request& req, httplib::Response& resp) {
                controller->set_error_callback(req, resp);
            });

        server->Post(SET_END_CALLBACK, [controller](const httplib::Request& req,
                                                    httplib::Response& resp) {
            controller->set_end_callback(req, resp);
        });
    }

    return server;
}
