#pragma once

#include <functional>

void Run(std::function<std::unique_ptr<httplib::Server>()> func);
