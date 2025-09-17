#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "util.h"
#include <fcntl.h>
#include <sys/epoll.h>
#include <errno.h>

#define MAX_EVENTS 1024
#define READ_BUFFER 1024

// 将socket fd设置为非阻塞模式
void setNoBlocking(int fd)
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    setNoBlocking(socket_fd);
    sockaddr_in serv_add;
    serv_add.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_add.sin_family = AF_INET;
    serv_add.sin_port = htons(8888);

    bind(socket_fd, reinterpret_cast<const sockaddr *>(&serv_add), sizeof(serv_add));

    listen(socket_fd, SOMAXCONN);

    int epfd = epoll_create1(0);

    epoll_event ev;
    epoll_event events[MAX_EVENTS];

    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = socket_fd;

    epoll_ctl(epfd, EPOLL_CTL_ADD, socket_fd, &ev);

    while (true)
    {
        int ndfs = epoll_wait(epfd, events, MAX_EVENTS, -1);
        for (int i = 0; i < ndfs; i++)
        {
            if (events[i].data.fd == socket_fd)
            {
                sockaddr_in client_add;
                socklen_t clinet_len = sizeof(client_add);
                bzero(&client_add, clinet_len);

                int client_fd = accept(socket_fd, reinterpret_cast<sockaddr *>(&client_add), &clinet_len);
                setNoBlocking(client_fd);

                ev.events = EPOLLIN;
                ev.data.fd = client_fd;

                epoll_ctl(epfd, EPOLL_CTL_ADD, client_fd, &ev);

                std::cout << "connected fd " << client_fd << ", addr " << inet_ntoa(client_add.sin_addr) << ", port " << ntohs(client_add.sin_port) << std::endl;
            }
            else if (events[i].events & EPOLLIN)
            {
                char buffer[READ_BUFFER];
                while (true)
                {
                    bzero(&buffer, sizeof(buffer));
                    ssize_t read_code = read(events[i].data.fd, &buffer, sizeof(buffer));
                    if (read_code > 0)
                    {
                        std::cout << "Received " << buffer << " from fd " << events[i].data.fd << std::endl;
                        ssize_t write_code = write(events[i].data.fd, &buffer, sizeof(buffer));

                        if (write_code == -1)
                        {
                            errif(true, "Error: write failed!");
                            break;
                        }
                    }
                    else if (read_code == 0)
                    {
                        std::cout << "Client fd " << events[i].data.fd << " disconnected!" << std::endl;
                        close(events[i].data.fd);
                        break;
                    }
                    else if ((read_code == -1) && (errno & EAGAIN) | (errno & EWOULDBLOCK)) // 非阻塞模式下，缓冲区无数据可读，返回-1，设置errno
                    {
                        std::cout << "Read once completed!" << std::endl;
                        break;
                    }
                    else if ((read_code == -1) && (errno & EINTR)) // 输入被中断的情况，continue继续读取输入
                    {
                        std::cout << "Read Interupted!" << std::endl;
                        continue;
                    }
                    else
                    {
                        std::cout << "Error: something undefined happened!" << std::endl;
                        close(events[i].data.fd);
                    }
                }
            }
            else
            {
                std::cout << "Error: something undefined happened!" << std::endl;
                close(events[i].data.fd);
            }
        }
    }

    close(socket_fd);
    return 0;
}