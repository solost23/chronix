#include "routers/main.h"

#include <iostream>

#include "chronix/chronix.h"
#include "global/constant/code.h"
#include "global/initialize/server.h"
#include "httplib/httplib.h"

static const std::string WEB_CONFIG_PATH = "config/config.yml";

static const size_t MIN_THREADS = 4;
static const size_t MAX_THREADS = 8 * std::thread::hardware_concurrency();

int main(int argc, char* argv[])
{
    // httplib::Server svr;
    // svr.Get("/hi", [](const Request & /*req*/, Response &res)
    // {
    //     res.set_content("Hello World!", "text/plain");
    // });

    // svr.listen("0.0.0.0", 8080);

    // 初始化
    // try
    // {
    //     auto scheduler =
    //         std::make_shared<ChronixScheduler>(MIN_THREADS, MAX_THREADS);
    // }
    // catch (const std::exception& e)
    // {
    //     std::cerr << e.what() << '\n';
    // }

    // 接口注册

    // 服务后台启动
    // 主程序循环等待
    // while (true)
    // {
    //     std::this_thread::sleep_for(std::chrono::seconds(10));
    // }

    // std::cout << BAD_REQUEST_CODE << std::endl;

    // 启动服务
    Run(Register);

    while (true)
    {
        std::this_thread::sleep_for(std::chrono::seconds(10));
    }

    return 0;
}
