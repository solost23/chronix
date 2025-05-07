#pragma once

#include "global/initialize/initialize.h"
#include "global/response/response.h"
#include "httplib/httplib.h"

static const std::string HEARTBEAT = "/";
static const std::string INSERT_CRON_TASK = "/api/chronix/cron";
static const std::string INSERT_ONCE_TASK = "/api/chronix/once";
static const std::string INSERT_IMMEDIATE_TASK = "/api/chronix/immediate";
static const std::string SET_START_CALLBACK = "/api/chronix/callback/start";
static const std::string SET_SUCCESS_CALLBACK = "/api/chronix/callback/success";
static const std::string SET_ERROR_CALLBACK = "/api/chronix/callback/error";
static const std::string SET_END_CALLBACK = "/api/chronix/callback/end";

std::unique_ptr<httplib::Server> Register(
    std::shared_ptr<Initialize>& initialize);
