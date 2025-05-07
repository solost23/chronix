#pragma once

#include "global/constant/code.h"
#include "httplib/httplib.h"
#include "nlohmann/json.hpp"

// template <typename T>
// struct Response
// {
//     int code;
//     T data;
//     std::string message;
//     bool Success;
// };

inline void error(httplib::Response& resp, int code, const std::exception& e)
{
    nlohmann::json j;

    j["code"] = code;
    j["success"] = false;
    j["message"] = e.what();
    j["data"] = "";

    resp.status = 200;
    resp.set_content(j.dump(), "Application/json");
}

template <typename T>
inline void success(httplib::Response& resp, T data)
{
    nlohmann::json j;

    j["code"] = 0;
    j["success"] = true;
    j["message"] = "";
    j["data"] = data;

    resp.status = 200;
    resp.set_content(j.dump(), "Application/json");
}
