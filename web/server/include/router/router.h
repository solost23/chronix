#pragma once

#include "global/initialize/initialize.h"
#include "global/response/response.h"
#include "httplib/httplib.h"

static const std::string HEARTBEAT = "/";
static const std::string INSERT_CRON = "/api/chronix/cron";
static const std::string INSERT_ONCE = "/api/chronix/once";
static const std::string INSERT_IMMEDIATE = "/api/chronix/immediate";
static const std::string SET_START_CALLBACK = "/api/chronix/callback/start";
static const std::string SET_SUCCESS_CALLBACK = "/api/chronix/callback/success";
static const std::string SET_ERROR_CALLBACK = "/api/chronix/callback/error";
static const std::string SET_END_CALLBACK = "/api/chronix/callback/end";
static const std::string SET_METRICS_ENABLED = "/api/chronix/metrics/enabled";
static const std::string REMOVE_JOB = "/api/chronix/remove";
static const std::string PAUSE_JOB = "/api/chronix/pause";
static const std::string RESUME_JOB = "/api/chronix/resume";
static const std::string STATUS_JOB = "/api/chronix/status";
static const std::string RESULT_JOB = "/api/chronix/result";
static const std::string METRICS_JOB = "/api/chronix/metrics";
static const std::string COUNT_JOB = "/api/chronix/count";
static const std::string COUNT_RUNNING_JOB = "/api/chronix/count/running";
static const std::string RUNNING = "/api/chronix/running";

std::unique_ptr<httplib::Server> Register(
    std::shared_ptr<Initialize>& initialize);
