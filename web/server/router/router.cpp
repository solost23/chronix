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
        server->Post(INSERT_CRON, [controller](const httplib::Request& req,
                                               httplib::Response& resp) {
            controller->insert_cron_task(req, resp);
        });

        server->Post(INSERT_ONCE, [controller](const httplib::Request& req,
                                               httplib::Response& resp) {
            controller->insert_once_task(req, resp);
        });

        server->Post(INSERT_IMMEDIATE, [controller](const httplib::Request& req,
                                                    httplib::Response& resp) {
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

        server->Put(
            SET_METRICS_ENABLED,
            [controller](const httplib::Request& req, httplib::Response& resp) {
                controller->set_metrics_enabled(req, resp);
            });

        server->Delete(REMOVE_JOB, [controller](const httplib::Request& req,
                                                httplib::Response& resp) {
            controller->remove_job(req, resp);
        });

        server->Delete(PAUSE_JOB, [controller](const httplib::Request& req,
                                               httplib::Response& resp) {
            controller->pause_job(req, resp);
        });

        server->Post(RESUME_JOB, [controller](const httplib::Request& req,
                                              httplib::Response& resp) {
            controller->resume_job(req, resp);
        });

        server->Get(STATUS_JOB, [controller](const httplib::Request& req,
                                             httplib::Response& resp) {
            controller->get_job_status(req, resp);
        });

        server->Get(RESULT_JOB, [controller](const httplib::Request& req,
                                             httplib::Response& resp) {
            controller->get_job_result(req, resp);
        });

        server->Get(METRICS_JOB, [controller](const httplib::Request& req,
                                              httplib::Response& resp) {
            controller->get_job_metrics(req, resp);
        });

        server->Get(COUNT_JOB, [controller](const httplib::Request& req,
                                            httplib::Response& resp) {
            controller->get_job_count(req, resp);
        });

        server->Get(COUNT_RUNNING_JOB, [controller](const httplib::Request& req,
                                                    httplib::Response& resp) {
            controller->get_running_job_count(req, resp);
        });

        server->Get(RUNNING, [controller](const httplib::Request& req,
                                          httplib::Response& resp) {
            controller->get_running(req, resp);
        });
    }

    return server;
}
