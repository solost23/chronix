#pragma once

#include <iostream>

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

private:
    const std::shared_ptr<Initialize> get_initialize();

    std::shared_ptr<Initialize> initialize;
};
