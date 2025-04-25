/*
 * main
 */
#include <thread>
#include <vector>

#include "example/example1.h"
#include "example/example2.h"
#include "example/example3.h"
#include "example/example4.h"
#include "example/example5.h"
#include "example/example6.h"
#include "example/example7.h"
#include "example/example8.h"
#include "example/example9.h"
#include "example/example10.h"

int main(int argc, char** argv)
{
    std::vector<std::thread> examples;
    examples.emplace_back(example1);
    examples.emplace_back(example2);
    examples.emplace_back(example3);
    examples.emplace_back(example4);
    examples.emplace_back(example5);
    examples.emplace_back(example6);
    examples.emplace_back(example7);
    examples.emplace_back(example8);
    examples.emplace_back(example9);
    examples.emplace_back(example10);
    
    for (auto& e : examples)
    {
        if (e.joinable())
        {
            e.join();
        }
    }

    return 0;
}
