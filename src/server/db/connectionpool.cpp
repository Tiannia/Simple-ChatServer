#include "connectionpool.hpp"
#include <unistd.h>
#include <muduo/base/Logging.h>

ConnectionPool *ConnectionPool::getConnectionPool()
{
	static ConnectionPool pool; // lock和unlock
	return &pool;
}

bool ConnectionPool::loadConfigFile()
{
	FILE *pf = fopen("../mysql.ini", "r");

	if (pf == nullptr)
	{
		char buf[80];
		getcwd(buf, sizeof(buf));
		LOG_DEBUG << "Current working directory is [" << buf << "]";
		LOG_ERROR << "mysql.ini file is not exist!";
		return false;
	}

	while (!feof(pf)) //文件没有到末尾
	{
		char line[1024] = {0};
		fgets(line, 1024, pf);
		string str = line;
		int idx = str.find('=', 0);
		if (idx == -1) //无效的配置项
		{
			continue;
		}

		int endidx = str.find('\n', idx);
		string key = str.substr(0, idx);
		string value = str.substr(idx + 1, endidx - idx - 1);

		if (key == "ip")
		{
			_ip = value;
		}
		else if (key == "port")
		{
			_port = atoi(value.c_str());
		}
		else if (key == "username")
		{
			_username = value;
		}
		else if (key == "password")
		{
			_password = value;
		}
		else if (key == "dbname")
		{
			_dbname = value;
		}
		else if (key == "initSize")
		{
			_initSize = atoi(value.c_str());
		}
		else if (key == "maxSize")
		{
			_maxSize = atoi(value.c_str());
		}
		else if (key == "maxIdleTime")
		{
			_maxIdleTime = atoi(value.c_str());
		}
		else if (key == "connectionTimeOut")
		{
			_connectionTimeout = atoi(value.c_str());
		}
	}

	fclose(pf);

	return true;
}

ConnectionPool::ConnectionPool() : _pool_alive(true)
{
	if (!loadConfigFile())
	{
		return;
	}

	for (int i = 0; i < _initSize; ++i)
	{
		Connection *p = new Connection();
		p->connect(_ip, _port, _username, _password, _dbname);
		p->refreshAliveTime();
		_connectionQue.push(p);
		++_connectionCnt;
	}

	//启动一个新的线程，作为连接的生产者 linux thread => pthread_create
	thread produce(std::bind(&ConnectionPool::produceConnectionTask, this));
	produce.detach();

	//启动一个新的定时线程，扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
	thread scanner(std::bind(&ConnectionPool::scannerConnectionTask, this));
	scanner.detach();
}

ConnectionPool::~ConnectionPool()
{
	{
		unique_lock<mutex> lock(_queueMutex);
		_is_closing = true;
		_cv_close.wait(lock, [this]
					   { return _use_count == 0; });
		while (!_connectionQue.empty())
		{
			Connection *p = _connectionQue.front();
			_connectionQue.pop();
			delete p;
		}
		_pool_alive = false;
	}
	_cv.notify_all();
}

//运行在独立的线程中，专门负责生产新连接
void ConnectionPool::produceConnectionTask()
{
	for (;;)
	{
		unique_lock<mutex> lock(_queueMutex);
		_cv.wait(lock, [this]
				 { return !this->_pool_alive || this->_connectionQue.empty(); });

		if (!_pool_alive)
			break;

		if (_connectionCnt < _maxSize)
		{
			Connection *p = new Connection();
			p->connect(_ip, _port, _username, _password, _dbname);
			p->refreshAliveTime();
			_connectionQue.push(p);
			++_connectionCnt;
		}

		_cv.notify_all();
	}
}

//给外部提供接口，从连接池中获取一个可用的空闲连接
shared_ptr<Connection> ConnectionPool::getConnection()
{
	unique_lock<mutex> lock(_queueMutex);

	while (_connectionQue.empty())
	{
		// sleep
		if (cv_status::timeout == _cv.wait_for(lock, chrono::milliseconds(_connectionTimeout))) // chrono::seconds() is second
		{
			if (_connectionQue.empty())
			{
				LOG_INFO << "Getting idle connections timed out. Failed to get connection!";
				return nullptr;
			}
		}
	}

	shared_ptr<Connection> sp(_connectionQue.front(),
							  [&](Connection *pcon)
							  {
								  unique_lock<mutex> lock(_queueMutex);
								  pcon->refreshAliveTime();
								  _connectionQue.push(pcon);
								  --_use_count;
								  if (_is_closing && _use_count == 0)
								  {
									  _cv_close.notify_all();
								  }
							  });
	_connectionQue.pop();
	++_use_count;
	_cv.notify_all();

	return sp;
}

//扫描超过maxIdleTime时间的空闲连接，进行对于的连接回收
void ConnectionPool::scannerConnectionTask()
{
	for (;;)
	{
		this_thread::sleep_for(chrono::seconds(_maxIdleTime));

		unique_lock<mutex> lock(_queueMutex);
		while (_connectionCnt > _initSize)
		{
			Connection *p = _connectionQue.front();
			if (p->getAliveeTime() >= (_maxIdleTime * 1000))
			{
				_connectionQue.pop();
				_connectionCnt--;
				delete p;
			}
			else
			{
				break;
			}
		}
	}
}
