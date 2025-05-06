#include "controller/controller.h"

void Controller::insert_cron_task(const httplib::Request& req, httplib::Response& resp)
{
    auto scheduler = get_initialize()->get_scheduler();
    scheduler->add_immediate_job([](){
        std::cout << "immediate_job" << std::endl; 
    });    

    success(resp, "");
}

const std::shared_ptr<Initialize> Controller::get_initialize()
{
    return initialize; 
}
