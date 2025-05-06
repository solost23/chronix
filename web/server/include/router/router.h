#pragma once

#include "global/initialize/initialize.h"
#include "httplib/httplib.h"
#include "global/response/response.h"

static const std::string HEARTBEAT = "/"; 
static const std::string INSERT_CRON_TASK = "/api/chronix/cron"; 

std::unique_ptr<httplib::Server> Register(std::shared_ptr<Initialize>& initialize);
