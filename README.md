## 集群聊天服务器
基于TCP协议实现的多人聊天服务器，支持单机/集群，包含客户端与服务端。

#### Tools：
- **Dev Platform**：vscode（ubuntu）、cmake、g++9.3.0
- **http Server**：nginx
- **Database**：redis、Mysql
- **Third Party**：muduo、JSON for Modern C++、hiredis

#### 模块组成：

- **网络模块**：基于moduo网络库构建本服务器的网络模块，解耦了网络模块的代码和业务层的代码，能够让开发者专注于业务层的开发。
- **业务模块**：作为服务层，旨在让代码简洁，优雅，可扩展。因此我使用了c++11的部分新特性，比如unordered_map，绑定器，函数对象，并做了一个回调机制：当这个网络I/O给服务器通知有请求消息，服务器就会执行我注册的读事件的回调操作`OnMessage`，于是我就可以从消息体里面(buffer)解析出这个json，得到消息的ID，通过回调操作，就可以调用相关的函数处理这个请求消息了。
- **数据存储层**：实现了一个MYSQL的数据库连接池，并通过书写model类对业务层封装底层数据库的操作，前者提高Mysql数据库的访问效率，后者提高了项目的扩展性，因为一个表单(table)对应一个model，如果需要增加表单，仅需在对应的model实现相应的功能即可。

#### 集群：

考虑项目整体的并发能力，让项目可以支持多机的扩展，并部署多台网络服务器，因此需要在最前面挂一台Nginx负载均衡，由于项目是基于TCP协议传输的C/S通讯模型，因此在Nginx的TCP负载均衡做了如下的配置：

```nginx
#nginx tcp loadbalance config
stream {
    upstream MyServer{
        server 127.0.0.1:6000 weight=1 max_fails=3 fail_timeout=30s;
        server 127.0.0.1:6002 weight=1 max_fails=3 fail_timeout=30s;
     }

    server {
        proxy_connect_timeout 1s;
        #proxy_timeout 3s;
        listen 8000;
        proxy_pass MyServer;
        tcp_nodelay on;
    }
}
```
**TCP连接要求**：长连接，因为是消息聊天通信，客户端不仅仅要给服务器主动发消息，而且服务器还要主动给客户端推送消息，所以必须是长连接。<br>
**服务器间通信要求**：在负载均衡里面，由于不同的服务器有不同的用户进行连接，他们之间需要通信的话，需要引入Redis作为MQ消息队列的功能（换别的也可以），利用它的发布订阅机制可以实现跨服务器通信的功能。

#### 使用：

1) 获取项目 && 构建项目

```shell
git clone git@github.com:Tiannia/Simple-ChatServer.git
cd Simple-ChatServer
./autobuild.sh
```
2) 将`chat.sql`的内容导入至Mysql服务器中，并启动Mysql服务
```shell
sudo service mysql start
```

3) 单机启动：

- 启动服务器：
```shell
cd bin
./ChatServer 127.0.0.1 6000
```
- 启动客户端：
```shell
./ChatClient 127.0.0.1 6000
```

4) 多服务器启动：
- 请确保Nginx已经启动，并且按照上述配置文件的内容配置完成
- 请确保Redis已经启动

- 启动服务器：
```shell
cd bin
./ChatServer 127.0.0.1 6000
./ChatServer 127.0.0.1 6002
```
- 启动客户端：
```shell
./ChatClient 127.0.0.1 8000
./ChatClient 127.0.0.1 8000
```

#### Demo:
![img](https://github.com/Tiannia/intro_to_ai/blob/main/PhotoRepository/demoofchatserver.png?raw=true)
![img](https://github.com/Tiannia/intro_to_ai/blob/main/PhotoRepository/processlistofmysqlconnection.png?raw=true)

