#include "offlinemessagemodel.hpp"
#include "sql_connection_pool.hpp"

//存储用户的离线消息
void OfflineMsgModel::insert(int userid, string msg)
{
    MySQL *mysql = nullptr;
    connectionRAII mysqlcon(&mysql, connection_pool::GetInstance());

    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into offlinemessage values(%d, '%s')", userid, msg.c_str());

    mysql->update(sql);//更新 
}

//删除用户的离线消息
void OfflineMsgModel::remove(int userid)
{
    MySQL *mysql = nullptr;
    connectionRAII mysqlcon(&mysql, connection_pool::GetInstance());

    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "delete from offlinemessage where userid=%d", userid);

    mysql->update(sql);//更新 
}

//查询用户的离线消息
vector<string> OfflineMsgModel::query(int userid)
{
    MySQL *mysql = nullptr;
    connectionRAII mysqlcon(&mysql, connection_pool::GetInstance());

    //1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select message from offlinemessage where userid = %d", userid);

    vector<string> vec;

    MYSQL_RES *res = mysql->query(sql);//调用mysql的查询 
    if (res != nullptr)
    {
        //把userid用户的所有离线消息放入vec中返回
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res)) != nullptr) //mysql_fetch_row() 从结果集合中取得下一行
        {
            vec.push_back(row[0]);//加入数组中 
        }
        mysql_free_result(res);//释放资源 
        return vec;
    }
    return vec;
}
