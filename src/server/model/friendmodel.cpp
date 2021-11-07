#include "friendmodel.hpp"
#include "sql_connection_pool.hpp"

//添加好友关系
void FriendModel::insert(int userid, int friendid)
{
    MySQL *mysql = nullptr;
    connectionRAII mysqlcon(&mysql, connection_pool::GetInstance());

    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "insert into friend values(%d, %d)", userid, friendid);

    mysql->update(sql);
}

//返回用户好友列表
vector<User> FriendModel::query(int userid)
{
    MySQL *mysql = nullptr;
    connectionRAII mysqlcon(&mysql, connection_pool::GetInstance());

    // 1.组装sql语句
    char sql[1024] = {0};
    sprintf(sql, "select a.id,a.name,a.state from user a inner join friend b on b.friendid = a.id where b.userid=%d", userid);

    vector<User> vec;
    MYSQL_RES *res = mysql->query(sql);
    if (res != nullptr)
    {
        //把userid的好友请求的用户信息都存入数组中 
        MYSQL_ROW row;
        while((row = mysql_fetch_row(res)) != nullptr)
        {
            User user;
            user.setId(atoi(row[0]));
            user.setName(row[1]);
            user.setState(row[2]);
            vec.push_back(user);
        }
        mysql_free_result(res);
        return vec;
    }
    return vec;
}
