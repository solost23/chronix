#include "controller/controller.h"

void Controller::insert_cron_task(const httplib::Request& req,
                                  httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        InsertCronTaskForm params = j.get<InsertCronTaskForm>();

        auto scheduler = get_initialize()->get_scheduler();
        size_t id = scheduler->add_cron_job(params.cron, [params]() {
            httplib::Client client(force_http(params.callback_url));

            client.set_connection_timeout(5);
            client.set_read_timeout(10);

            auto result = client.Get(params.callback_url);
            std::cout << "Callback result: " << result->status << std::endl;
            if (result && result->status == 200)
            {
                std::clog << "[Info] success send callback to "
                          << params.callback_url << std::endl;
            }
            else
            {
                std::clog << "[Info] failed send callback to "
                          << params.callback_url << std::endl;
                std::clog << "[Info] callback response: " << result->body
                          << std::endl;
            }
        });

        success(resp, id);
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::insert_once_task(const httplib::Request& req,
                                  httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        InsertOnceTaskForm params = j.get<InsertOnceTaskForm>();

        auto tp = parse_iso_time(params.run_at);

        auto scheduler = get_initialize()->get_scheduler();

        size_t id = scheduler->add_once_job(tp, [params]() {
            httplib::Client client(force_http(params.callback_url));

            client.set_connection_timeout(5);
            client.set_read_timeout(10);

            auto result = client.Get(params.callback_url);
            std::cout << "Callback result: " << result->status << std::endl;
            if (result && result->status == 200)
            {
                std::clog << "[Info] success send callback to "
                          << params.callback_url << std::endl;
            }
            else
            {
                std::clog << "[Info] failed send callback to "
                          << params.callback_url << std::endl;
                std::clog << "[Info] callback response: " << result->body
                          << std::endl;
            }
        });
        success(resp, id);
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::insert_immediate_task(const httplib::Request& req,
                                       httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        InsertImmediateTaskForm params = j.get<InsertImmediateTaskForm>();

        auto scheduler = get_initialize()->get_scheduler();
        size_t id = scheduler->add_immediate_job([params]() {
            httplib::Client client(force_http(params.callback_url));

            client.set_connection_timeout(5);
            client.set_read_timeout(10);

            auto result = client.Get(params.callback_url);
            std::cout << "Callback result: " << result->status << std::endl;
            if (result && result->status == 200)
            {
                std::clog << "[Info] success send callback to "
                          << params.callback_url << std::endl;
            }
            else
            {
                std::clog << "[Info] failed send callback to "
                          << params.callback_url << std::endl;
                std::clog << "[Info] callback response: " << result->body
                          << std::endl;
            }
        });

        success(resp, id);
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

const std::shared_ptr<Initialize> Controller::get_initialize()
{
    return initialize;
}
