#include "charserver.hpp"
#include "charservice.hpp"
#include "sql_connection_pool.hpp"
#include <iostream>
#include <signal.h>
using namespace std;

//数据库配置信息
static string m_server = "127.0.0.1";
static string m_user = "root";
static string m_password = "123456";
static string m_dbname = "chat";
static constexpr int m_sql_num = 6; //最大连接数


//处理服务器ctrl+c结束后，进行重置user的状态信息
void resetHandler(int)
{
    ChatService::instance()->reset();//调用重置 
    exit(0);
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

    //初始化数据库连接池
    connection_pool* m_connPool = connection_pool::GetInstance(); //懒汉模式
    m_connPool->init(m_server, m_user, m_password, m_dbname, 3306, m_sql_num);

    server.start();
    loop.loop();

    return 0;
}
