#include "routers/main.h"

#include <iostream>

#include "chronix/chronix.h"
#include "global/constant/code.h"
#include "global/global.h"
#include "global/initialize/main.h"
#include "global/initialize/server.h"

static const std::string WEB_CONFIG_PATH = "config/config.yml";

int main(int argc, char* argv[])
{
    try
    {
        // 项目初始化
        initialize(WEB_CONFIG_PATH);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }

    // std::cout << server_config.get_name() << std::endl;

    // 启动服务
    Run(Register);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}
