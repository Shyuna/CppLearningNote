- 添加头文件`#include <fcntl.h>`

  `#include <sys/epoll.h>`

  `#include <errno.h>`
- 使用`fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);`将socket fd设置为非阻塞模式
- 创建epoll文件描述符：`int epfd = epoll_create1(0);`

  将IO口放入epoll红黑树中，监听IO上的事件：`epoll_ctl(epfd, EPOLL_CTL_ADD, sockfd, &ev);`

  获取有事件发生的fd：`int nfds = epoll_wait(epfd, events, MAX_EVENTS, -1);`
- `read()`函数返回-1时，通过`(errno == EAGAIN) || (errno == EWOULDBLOCK)`判断是否是非阻塞IO数据读取完毕，继续使用`epoll_wait`等待
- `read()`函数返回0时，使用`close(events[i].data.fd);`关闭socket，自动将文件描述符从epoll树上移除
