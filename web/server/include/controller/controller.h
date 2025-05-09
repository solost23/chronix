#pragma once

#include <iostream>
#include <string>

#include "form/form.h"
#include "global/initialize/initialize.h"
#include "global/response/response.h"
#include "httplib/httplib.h"

class Controller
{
public:
    Controller(const std::shared_ptr<Initialize>& initialize)
        : initialize(initialize)
    {}

    // api
    void insert_cron_task(const httplib::Request& req, httplib::Response& resp);
    void insert_once_task(const httplib::Request& req, httplib::Response& resp);
    void insert_immediate_task(const httplib::Request& req,
                               httplib::Response& resp);
    void set_start_callback(const httplib::Request& req,
                            httplib::Response& resp);
    void set_success_callback(const httplib::Request& req,
                              httplib::Response& resp);
    void set_error_callback(const httplib::Request& req,
                            httplib::Response& resp);
    void set_end_callback(const httplib::Request& req, httplib::Response& resp);
    void set_metrics_enabled(const httplib::Request& req,
                             httplib::Response& resp);
    void remove_job(const httplib::Request& req, httplib::Response& resp);
    void pause_job(const httplib::Request& req, httplib::Response& resp);
    void resume_job(const httplib::Request& req, httplib::Response& resp);
    void get_job_status(const httplib::Request& req, httplib::Response& resp);
    void get_job_result(const httplib::Request& req, httplib::Response& resp);
    void get_job_metrics(const httplib::Request& req, httplib::Response& resp);
    void get_job_count(const httplib::Request& req, httplib::Response& resp);
    void get_running_job_count(const httplib::Request& req,
                               httplib::Response& resp);
    void get_running(const httplib::Request& req, httplib::Response& resp);

private:
    const std::shared_ptr<Initialize> get_initialize();

    std::shared_ptr<Initialize> initialize;
};
