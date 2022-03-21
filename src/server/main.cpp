#include "charserver.hpp"
#include "charservice.hpp"
#include "connectionpool.hpp"
#include <iostream>
#include <signal.h>
using namespace std;

//处理服务器ctrl+c结束后，进行重置user的状态信息
void resetHandler(int)
{
    ChatService::instance()->reset(); //调用重置
    exit(0);                          //进程退出
}

int main(int argc, char **argv)
{

    if (argc < 3)
    {
        cerr << "command invalid! example: ./ChatServer 127.0.0.1 6000" << endl;
        exit(-1);
    }

    //解析通过命令行参数传递的ip和port
    char *ip = argv[1];
    uint16_t port = atoi(argv[2]);

    signal(SIGINT, resetHandler);

    EventLoop loop;
    InetAddress addr(ip, port);
    ChatServer server(&loop, addr, "ChatServer");

    server.start();
    loop.loop();

    return 0;
}
