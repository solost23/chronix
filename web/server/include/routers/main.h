#pragma once

#include "httplib/httplib.h"

std::unique_ptr<httplib::Server> Register();
