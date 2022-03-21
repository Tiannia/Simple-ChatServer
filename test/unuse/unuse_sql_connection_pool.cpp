#include <stdlib.h>
#include <pthread.h>
#include <muduo/base/Logging.h>
#include "unuse_sql_connection_pool.hpp"

using namespace std;

connection_pool::connection_pool()
{
	m_CurConn = 0;
	m_FreeConn = 0;
}

connection_pool *connection_pool::GetInstance()
{
	static connection_pool connPool;
	return &connPool;
}

//构造初始化
void connection_pool::init(string url, string User, string PassWord, string DBName, int Port, int MaxConn)
{
	m_url = url;
	m_Port = Port;
	m_User = User;
	m_PassWord = PassWord;
	m_DatabaseName = DBName;

	//创建MaxConn条数据库连接
	for (int i = 0; i < MaxConn; i++)
	{
		MySQL *con = new MySQL();

		if(con->connect(url.c_str(), User.c_str(), PassWord.c_str(), DBName.c_str(), Port)){
			//创建MaxConn条数据库连接
			connList.push_back(con);
			++m_FreeConn;
		}
	}

	//将信号量初始化为最大连接次数
	reserve = sem(m_FreeConn);

	m_MaxConn = m_FreeConn;
}

/*
	由于多线程操作连接池，会造成竞争，这里使用互斥锁完成同步，
	具体的同步机制均使用lock.h中封装好的类。
*/
//当有请求时，从数据库连接池中返回一个可用连接，更新使用和空闲连接数
MySQL *connection_pool::GetConnection()
{
    assert(connList.size() > 0);
	MySQL *con = NULL;
	
	reserve.wait();
	
	lock.lock();

	con = connList.front();
	connList.pop_front();

	--m_FreeConn;
	++m_CurConn;

	lock.unlock();
	return con;
}

//释放当前使用的连接
bool connection_pool::ReleaseConnection(MySQL *con)
{
	if (NULL == con)
		return false;

	lock.lock();

	connList.push_back(con);
	++m_FreeConn;
	--m_CurConn;

	lock.unlock();

	reserve.post();
	return true;
}

//销毁数据库连接池
void connection_pool::DestroyPool()
{

	lock.lock();
	if (connList.size() > 0)
	{
		list<MySQL *>::iterator it;
		for (it = connList.begin(); it != connList.end(); ++it)
		{
			MySQL *con = *it;
			delete con;
		}
		m_CurConn = 0;
		m_FreeConn = 0;

		//清空list
		connList.clear();
	}
	lock.unlock();
}

//当前空闲的连接数
int connection_pool::GetFreeConn()
{
	return this->m_FreeConn;
}

connection_pool::~connection_pool()
{
	DestroyPool();
}

//不直接调用获取和释放连接的接口，将其封装起来，通过RAII机制进行获取和释放。
connectionRAII::connectionRAII(MySQL **SQL, connection_pool *connPool){
	//需要对SQL指向的连接进行赋值，即获取连接池的连接，因此需要使用双指针，因为操作的是实际地址。
	*SQL = connPool->GetConnection(); 
	
	conRAII = *SQL;
	poolRAII = connPool;
}

connectionRAII::~connectionRAII(){
	poolRAII->ReleaseConnection(conRAII);
}