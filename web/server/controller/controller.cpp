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
            auto [host, path] =
                extract_host_and_path(force_http(params.callback_url));
            httplib::Client client(host);

            client.set_connection_timeout(5);
            client.set_read_timeout(10);

            auto result = client.Get(path);
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
            auto [host, path] =
                extract_host_and_path(force_http(params.callback_url));

            httplib::Client client(host);

            client.set_connection_timeout(5);
            client.set_read_timeout(10);

            auto result = client.Get(path);
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
            auto [host, path] =
                extract_host_and_path(force_http(params.callback_url));

            httplib::Client client(host);

            client.set_connection_timeout(5);
            client.set_read_timeout(10);

            auto result = client.Get(path);
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

void Controller::set_start_callback(const httplib::Request& req,
                                    httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        SetCallbackForm params = j.get<SetCallbackForm>();

        std::string separator =
            (params.callback_url.find('?') == std::string::npos) ? "?" : "&";
        params.callback_url = force_http(params.callback_url) + separator +
                              "id=" + std::to_string(params.id);

        auto scheduler = get_initialize()->get_scheduler();

        scheduler->set_start_callback(params.id, [params](size_t id) {
            auto [host, path] =
                extract_host_and_path(force_http(params.callback_url));

            httplib::Client client(host);

            client.set_connection_timeout(5);
            client.set_read_timeout(10);

            auto result = client.Get(path);
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

        success(resp, "");
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::set_success_callback(const httplib::Request& req,
                                      httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        SetCallbackForm params = j.get<SetCallbackForm>();

        std::string separator =
            (params.callback_url.find('?') == std::string::npos) ? "?" : "&";
        params.callback_url = force_http(params.callback_url) + separator +
                              "id=" + std::to_string(params.id);

        auto scheduler = get_initialize()->get_scheduler();

        scheduler->set_success_callback(params.id, [params](size_t id) {
            auto [host, path] =
                extract_host_and_path(force_http(params.callback_url));

            httplib::Client client(host);

            client.set_connection_timeout(5);
            client.set_read_timeout(10);

            auto result = client.Get(path);
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
        success(resp, "");
    }
    catch (const std::exception& e)
    {
        error(resp, 500, e);
    }
}

void Controller::set_error_callback(const httplib::Request& req,
                                    httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        SetCallbackForm params = j.get<SetCallbackForm>();

        std::string separator =
            (params.callback_url.find('?') == std::string::npos) ? "?" : "&";
        params.callback_url = force_http(params.callback_url) + separator +
                              "id=" + std::to_string(params.id);

        auto scheduler = get_initialize()->get_scheduler();

        scheduler->set_error_callback(
            params.id, [params](size_t id, const std::exception& e) {
                auto [host, path] =
                    extract_host_and_path(force_http(params.callback_url));
                httplib::Client client(host);

                client.set_connection_timeout(5);
                client.set_read_timeout(10);

                auto result = client.Get(path);
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
        success(resp, "");
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::set_end_callback(const httplib::Request& req,
                                  httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        SetCallbackForm params = j.get<SetCallbackForm>();

        std::string separator =
            (params.callback_url.find('?') == std::string::npos) ? "?" : "&";
        params.callback_url = force_http(params.callback_url) + separator +
                              "id=" + std::to_string(params.id);

        auto scheduler = get_initialize()->get_scheduler();

        scheduler->set_end_callback(params.id, [params](size_t id) {
            auto [host, path] =
                extract_host_and_path(force_http(params.callback_url));

            httplib::Client client(host);

            client.set_connection_timeout(5);
            client.set_read_timeout(10);

            auto result = client.Get(path);
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
        success(resp, "");
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::set_metrics_enabled(const httplib::Request& req,
                                     httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        SetMetricsEnabledForm params = j.get<SetMetricsEnabledForm>();

        auto scheduler = get_initialize()->get_scheduler();

        scheduler->set_metrics_enabled(params.enabled);
        success(resp, params.enabled);
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::remove_job(const httplib::Request& req,
                            httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        IDForm params = j.get<IDForm>();

        auto scheduler = get_initialize()->get_scheduler();

        scheduler->remove_job(params.id);
        success(resp, "");
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::pause_job(const httplib::Request& req, httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        IDForm params = j.get<IDForm>();

        auto scheduler = get_initialize()->get_scheduler();

        scheduler->pause_job(params.id);
        success(resp, "");
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::resume_job(const httplib::Request& req,
                            httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        IDForm params = j.get<IDForm>();

        auto scheduler = get_initialize()->get_scheduler();

        scheduler->resume_job(params.id);
        success(resp, "");
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::get_job_status(const httplib::Request& req,
                                httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        IDForm params = j.get<IDForm>();

        auto scheduler = get_initialize()->get_scheduler();

        auto status = scheduler->get_job_status(params.id);
        success(resp, scheduler->status_to_string(status));
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::get_job_result(const httplib::Request& req,
                                httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        IDForm params = j.get<IDForm>();

        auto scheduler = get_initialize()->get_scheduler();

        auto result = scheduler->get_job_result(params.id);
        success(resp, scheduler->result_to_string(result));
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::get_job_metrics(const httplib::Request& req,
                                 httplib::Response& resp)
{
    try
    {
        nlohmann::json j = nlohmann::json::parse(req.body);
        IDForm params = j.get<IDForm>();

        auto scheduler = get_initialize()->get_scheduler();

        auto metrics = scheduler->get_job_metrics(params.id);

        Metrics result;
        {
            result.execution_count = metrics.execution_count;
            result.success_count = metrics.success_count;
            result.error_count = metrics.error_count;

            result.last_run_time = to_iso_time(metrics.last_run_time);
            result.last_duration = metrics.last_duration.count();
            result.total_duration = metrics.total_duration.count();
            result.max_duration = metrics.max_duration.count();
            result.min_duration = metrics.min_duration.count();

            for (auto& success_time : metrics.success_times)
            {
                result.success_times.push_back(to_iso_time(success_time));
            }
            for (auto& error_time : metrics.error_times)
            {
                result.error_times.push_back(to_iso_time(error_time));
            }

            result.description = metrics.description;

            result.average_duration = metrics.average_duration().count();
            result.success_rate = metrics.success_rate();
            result.error_rate = metrics.error_rate();
        }

        success(resp, result);
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::get_job_count(const httplib::Request& req,
                               httplib::Response& resp)
{
    try
    {
        auto scheduler = get_initialize()->get_scheduler();
        success(resp, scheduler->get_job_count());
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::get_running_job_count(const httplib::Request& req,
                                       httplib::Response& resp)
{
    try
    {
        auto scheduler = get_initialize()->get_scheduler();
        success(resp, scheduler->get_running_job_count());
    }
    catch (const std::exception& e)
    {
        error(resp, INTERNAL_SERVER_ERROR_CODE, e);
    }
}

void Controller::get_running(const httplib::Request&, httplib::Response& resp)
{
    try
    {
        auto scheduler = get_initialize()->get_scheduler();
        success(resp, scheduler->get_running());
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
