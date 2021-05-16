# 文件相关编程知识

## Open

open 可以接受两个参数，也可以接受三个参数，两个参数即为文件的绝对路径和以什么状态打开文件，三个参数中前两个参数与之前相同只不过多了一个表示文件访问权限的初始值参数其定义如下：

```CPP
int open(const char *pathname, int flags);
int open(const char *pathname, int flags, mode_t mode);
```

open 还有一个与之功能相近的函数 openat 函数该函数支持以相对路径指定文件，其余部分与 open 相同，其定义如下：

```CPP
int openat(int dirfd, const char *pathname, int flags);
int openat(int dirfd, const char *pathname, int flags, mode_t mode);
```

有一点需要注意的是，只有在 flags 中带有 O_CREAT 时第三个参数才带有意义，否则第三个参数会被直忽略

flags 参数含义如下：

- O_RDONLY：只读模式
- O_WRONLY：只写模式
- O_RDWR：可读可写
- O_APPEND 表示追加，如果原来文件里面有内容，则这次写入会写在文件的最末尾。
- O_CREAT 表示如果指定文件不存在，则创建这个文件
- O_EXCL 表示如果要创建的文件已存在，则出错，同时返回 -1，并且修改 errno 的值。
- O_TRUNC 表示截断，如果文件存在，并且以只写、读写方式打开，则将其长度截断为0。
- O_NOCTTY 如果路径名指向终端设备，不要把这个设备用作控制终端。
- O_NONBLOCK 如果路径名指向 FIFO/块文件/字符文件，则把文件的打开和后继 I/O设置为非阻塞模式（nonblocking mode）
- O_DSYNC 等待物理 I/O 结束后再 write。在不影响读取新写入的数据的前提下，不等待文件属性更新。
- O_RSYNC read 等待所有写入同一区域的写操作完成后再进行
- O_SYNC 等待物理 I/O 结束后再 write，包括更新文件属性的 I/O
