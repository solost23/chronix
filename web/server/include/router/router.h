#pragma once

#include "global/initialize/initialize.h"
#include "global/response/response.h"
#include "httplib/httplib.h"

static const std::string HEARTBEAT = "/";
static const std::string INSERT_CRON_TASK = "/api/chronix/cron";
static const std::string INSERT_ONCE_TASK = "/api/chronix/once";
static const std::string INSERT_IMMEDIATE_TASK = "/api/chronix/immediate";

std::unique_ptr<httplib::Server> Register(
    std::shared_ptr<Initialize>& initialize);
