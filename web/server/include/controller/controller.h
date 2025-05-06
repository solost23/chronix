#pragma once 

#include "global/initialize/initialize.h"

class Controller
{
public:
    Controller(const std::shared_ptr<Initialize>& initialize)
        : initialize(initialize)
    {}

    // api 

private:
    std::shared_ptr<Initialize> initialize;
}; 
