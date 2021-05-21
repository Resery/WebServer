# Echo Server Note

这是第一次在学习了套接字之后实现的一个简易的套接字程序，主要记录一下相关的知识点，主要是几个数据结构和函数的作用，以及建立的过程来为网络编程打下基础

## 数据结构

sockaddr_in

sockaddr_in是IPV4套接字地址结构，它在不同系统中具体定义可能有所不同：

```CPP
struct sockaddr_in{
    sa_family_t sin_family;
    in_port_t sin_port;
    struct in_addr sin_addr;
    char sin_zero[8];
};
```
但它们都包含三个基本的成员：

- sin_family 协议族
- sin_port 协议端口
- sin_addr 协议地址
协议族通常有以下几种类型：

- AF_INET IPV4协议
- AF_INET6 IPV6协议
- AF_LOCAL Unix域协议
- AF_ROUTE 路由套接字
- AF_KEY 秘钥套接字
- ......

## 相关函数

htons/ntohs
htons/ntohs这两个宏分别用于将本地字节序转为网络字节序和将网络字节序转为本地字节序。

inet_pton/inet_ntop
inet_pton/inet_ntop分别用于将字符串ip地址转为4字节大小的无符号整型和将无符号整型转换为ip地址字符串。

## 建立过程

socket--确定协议族和套接字类型

调用socket函数是执行网络I/O之前必须做的一件事情。通过socket函数指定了本次网络通信的协议族，套接字类型，调用成功后，会返回一个非负的套接字描述符，否则返回-1，具体失败原因，被存放于全局变量errno。它和文件描述类似，只不过此时它还不能进行正常的网络读写。
socket函数相关信息如下：

```CPP
#include<sys/socket.h>
int socket(int family,int type,int protocol);
```

bind--指定套接字地址信息

调用socket函数之后已经确定了协议族和传输协议，但是还没有确定本地协议，即套接字地址信息。bind函数描述如下：

```CPP
#include<sys/socket.h>
int bind(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
```

listen--监听客户端连接

listen函数用于将前面得到的套接字变为一个被动套接字，即可用于接受来自客户端的连接。描述如下：

```CPP
#include<sys/socket.h>
int listen(int sockfd,int backlog);
```

connect--建立连接

connect函数在客户端调用，它用来与服务端建立连接。描述如下：

```CPP
#include<sys/socket.h>
int connect(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
返回0表明成功，-1表明失，具体失败原因，被存放于全局变量errno。connect函数的参数与bind函数一样，这里就不多做解释了，只不过addr指明的是远端协议地址。如果本次连接是TCP协议，则connect函数调用将会发起TCP的三次握手。
```

accept--接受来自客户端的连接

accept函数在服务端调用，它用于接受来自客户端的连接，从已完成连接队列返回一个已完成连接。描述如下：

```CPP
#include<sys/socket.h>
int accept(int sockfd,const struct sockaddr *addr,socklen_t addrlen);
```

成功返回非负套接字描述符，失败返回-1，具体失败原因，被存放于全局变量errno。需要注意的是accept函数参数类型和数量与connect函数一致，但是含义不同，addr用于获取客户端的套接字地址信息，如果不关心客户端的协议地址，那么该参数可为NULL。

另外需要注意的是，它的返回值是一个非负的套接字描述符，这个套接字描述符是已连接套接字描述符，而其参数sockfd是监听套接字描述符
