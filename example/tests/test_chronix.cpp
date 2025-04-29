#include <cassert>
#include <iostream>

#include "chronix/chronix.h"

int main(int argc, char** argv)
{
    auto scheduler = std::make_shared<ChronixScheduler>(4);

    bool run{false};
    try
    {
        scheduler->start();

        scheduler->add_immediate_job([&]() { run = true; });
    }
    catch (std::exception& e)
    {
        std::cout << "Error: " << e.what() << std::endl;
    }

    std::this_thread::sleep_for(std::chrono::seconds(1));

    assert(run && "❌ Task did not run!");
    std::cout << "✅ Task ran!" << std::endl;

    return 0;
}
