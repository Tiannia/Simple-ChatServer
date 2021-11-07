#include "db.hpp"
#include <muduo/base/Logging.h>

//数据库配置信息

//初始化数据库连接
MySQL::MySQL()
{
    _conn = mysql_init(nullptr);
    if (nullptr == _conn)
    {
        LOG_INFO << "Init mysql fail!";
    }
}

//释放数据库连接资源
MySQL::~MySQL()
{
    if (_conn != nullptr)
        mysql_close(_conn);
}

//连接数据库
bool MySQL::connect(string url, string User, string PassWord, string DBName, int Port)
{
    MYSQL *p = mysql_real_connect(_conn, url.c_str(), User.c_str(),
                                  PassWord.c_str(), DBName.c_str(), Port, nullptr, 0);
    if (p != nullptr)
    {
        //C和C++代码默认的编码字符是ASCII，如果不设置，从MySQL上拉下来的中文显示？
        mysql_query(_conn, "set names gbk");
        LOG_INFO << "Connect mysql success!";
    }
    else
    {
        LOG_INFO << "Connect mysql fail!";
    }

    return p;
}

//更新操作
bool MySQL::update(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "更新失败!";
        return false;
    }

    return true;
}

//查询操作
MYSQL_RES *MySQL::query(string sql)
{
    if (mysql_query(_conn, sql.c_str()))
    {
        LOG_INFO << __FILE__ << ":" << __LINE__ << ":"
                 << sql << "查询失败!";
        return nullptr;
    }

    return mysql_use_result(_conn);
}

//获取连接
MYSQL *MySQL::getConnection()
{
    return _conn;
}
