# WebServer 技术文档

## 简要介绍

### Server 类：

描述：
Server 类主要用来记录跟服务器本身相关的信息，以及查看服务器信息的基本功能

成员变量：

ServerAddr: ServerAddr 就是主要用来存储服务器的相关信息，例如服务器使用的网络协议，IP地址，端口等信息

成员函数：

GetServerAddr: 返回创建的对象的私有变量 ServerAddr

PrintServerAddr: 输出与服务器相关的信息，如服务器的 IP 地址使用的端口信息等

### HTTPHandler 类：

描述：
HTTPHandler 类主要用来处理接收到的**请求**和发送的**响应**

成员变量：

ClientFd: 该变量用来存储连接的客户端的套接字文件描述符

Static: 该变量用来存储此次请求是请求静态资源还是动态资源

Method: 该变量用来存储客户端使用的哪种请求

Path: 该变量用来存储 URI

Version: 该变量用来存储请求的 HTTP 协议版本

CgiArgs: 该变量用来存储调用的 Cgi 程序的参数

FileName: 该变量用来存储请求的资源文件或 Cgi 程序的路径

FileType: 该变量用来存储需要响应的文件的类型

ErrorType: 该变量用来存储产生的错误类型

ReqMethod: 该变量用来存储客户端使用的是什么请求

ResponseBody: 该变量用来存储返回给客户端的响应的内容

成员函数:

GetClientFd: 获取连接的客户端的套接字文件描述符

GetMethod: 获取客户端的请求方法

GetPath: 获取客户端请求的资源的地址

GetVersion: 获取客户端请求的 HTTP 版本

GetResponseBody: 获取返回给客户端的响应的内容

GetFileType: 获取响应的文件的类型

ParseFileType: 解析出响应文件的文件类型

ReadRequest: 读取客户端发送的请求

ParseUri: 解析 Uri

SendResponse: 向客户端发送响应

SendErrorResponse: 向客户端发送错误响应，旨在告诉客户端发生了何种错误

Check: 检测请求是否合法

CheckMethod: 检测是否支持客户端的请求

CheckPath: 检测客户端请求的资源是否存在或者是否有对应的权限足以访问

CheckVersion: 检测是否支持客户端使用的 HTTP 协议版本

HandleError: 检测是发生了何种错误

## ToDo

支持并发