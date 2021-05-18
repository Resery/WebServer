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
- O_EXCL 表示如果要创建的文件已存在，则出错，同时返回 -1，并且修改 errno 的值。并且需要注意只有在 EXCL 与 CREAT 配合使用时才会有前述的功能
- O_TRUNC 表示截断，如果文件存在，并且以只写、读写方式打开，则将其长度截断为0。
- O_NOCTTY 如果路径名指向终端设备，不要把这个设备用作控制终端。
- O_NONBLOCK 如果路径名指向 FIFO/块文件/字符文件，则把文件的打开和后继 I/O设置为非阻塞模式（nonblocking mode）
- O_DSYNC 等待物理 I/O 结束后再 write。在不影响读取新写入的数据的前提下，不等待文件属性更新。
- O_RSYNC read 等待所有写入同一区域的写操作完成后再进行
- O_SYNC 等待物理 I/O 结束后再 write，包括更新文件属性的 I/O

Open/Open.cpp 中有关于前 7 个 flag 的相关例子

## Lseek

lseek 函数用于修改文件当前偏移，其接受三个参数第一个参数为文件描述符 fd，第二个参数为相对于 whence 的当前偏移，第三个参数为一个类似于标志的东西，其定义如下：

```CPP
off_t lseek(int fd, off_t offset, int whence);
```

whence 参数含义如下：

- SEEK_SET: 设置偏移为 文件开头 +/- offset
- SEEK_CUR: 设置偏移为 当前位置 +/- offset
- SEEK_END: 设置偏移为 文件结尾 +/- offset
- SEEK_DATA: 将偏移量移动到下一个包含数据的位置，该位置大于等于 offset 。如果 offset 已经指向数据，那么将偏移量设置为 offset 。
- SEEK_HOLE: 将偏移量移动到下一个包含空洞的位置，该位置大于等于 offset 。如果 offset 已经指向空洞，那么将偏移量设置为 offset 。如果在 offset 之后没有任何空洞，那么偏移量设置为文件的最末尾。（文件末尾也被视为是一个空洞，因为它同样以 0 结尾。）

## Dup And Dup2

两个函数定义如下:

```CPP
int dup(int oldfd);
int dup2(int oldfd, int newfd);
```

dup 是复制 oldfd 给一个新的 newfd，newfd 的值是当前最小可用的 fd，dup2 是复制 oldfd 到指定到 newfd，但是如果 newfd 的值存在的话就会直接返回，如果 oldfd 是个无效的值那么就会产生一个错误
