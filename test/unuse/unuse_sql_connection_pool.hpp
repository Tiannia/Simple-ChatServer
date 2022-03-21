#pragma once
#include <list>
#include <error.h>
#include <string>
#include <iostream>
#include "unuse_db.hpp"
#include "locker.hpp"

using namespace std;

class connection_pool
{
public:
	MySQL *GetConnection();				 //获取数据库连接
	bool ReleaseConnection(MySQL *conn); //释放连接
	int GetFreeConn();					 //获取连接
	void DestroyPool();					 //销毁所有连接

	//单例模式
	static connection_pool *GetInstance();

	void init(string url, string User, string PassWord, string DataBaseName, int Port, int MaxConn); 

private:
	connection_pool();
	~connection_pool();

	int m_MaxConn;  //最大连接数
	int m_CurConn;  //当前已使用的连接数
	int m_FreeConn; //当前空闲的连接数
	locker lock;
	list<MySQL *> connList; //连接池
	sem reserve;	        //信号量，实现对连接池资源的共享使用

public:
	string m_url;		   //主机地址
	string m_Port;		   //数据库端口号
	string m_User;		   //登陆数据库用户名
	string m_PassWord;	   //登陆数据库密码
	string m_DatabaseName; //使用数据库名
};


//通过RAII机制封装——数据库连接的-获取-与-释放-
class connectionRAII{

public:
	//双指针对MYSQL *con修改
	connectionRAII(MySQL **con, connection_pool *connPool);
	~connectionRAII();
	
private:
	MySQL *conRAII;
	connection_pool *poolRAII;
};

