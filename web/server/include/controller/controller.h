#pragma once 

#include "httplib/httplib.h"
#include "global/initialize/initialize.h"
#include "global/response/response.h"

class Controller
{
public:
    Controller(const std::shared_ptr<Initialize>& initialize)
        : initialize(initialize)
    {}

    // api 
    void insert_cron_task(const httplib::Request& req, httplib::Response& resp); 

private:
    const std::shared_ptr<Initialize> get_initialize(); 

    std::shared_ptr<Initialize> initialize;
}; 
