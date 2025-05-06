#pragma once

#include <functional>
#include <httplib/httplib.h> 
#include "global/initialize/initialize.h"

void Run(std::function<std::unique_ptr<httplib::Server>(std::shared_ptr<Initialize>& initialize)> func, std::shared_ptr<Initialize>& initialize);
