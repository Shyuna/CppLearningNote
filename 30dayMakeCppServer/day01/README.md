- 头文件：`#include <sys/socket.h>`


`#include <arpa/inet.h>`



- 地址存储在`sockaddr_in`结构中，使用函数时需要转换为`const sockaddr *`




- 设置`serv_add.sin_port`时，使用`htons()`将主机序转换为网络序，server接收到的client的端口号，使用`ntohs()`将网络序转换为主机序。




- server: socket->bind->listen->accept;


- client: socket->connect;