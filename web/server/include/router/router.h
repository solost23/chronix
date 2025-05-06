#pragma once

#include "global/initialize/initialize.h"
#include "httplib/httplib.h"

std::unique_ptr<httplib::Server> Register(std::shared_ptr<Initialize>& initialize);
