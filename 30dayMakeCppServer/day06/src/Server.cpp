#include "Server.h"
#include <functional>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <vector>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include <cstring>
#include "Channel.h"

#define READ_BUFFER 1024

Server::Server(EventLoop *loop) : loop(loop)
{
    Socket *server_sock = new Socket();
    server_sock->setnonblocking();

    InetAddress *server_addr = new InetAddress("127.0.0.1", 8888);
    server_sock->bind(server_addr);
    server_sock->listen();

    Channel *server_channel = new Channel(loop, server_sock->getFd());
    server_channel->enableReading();
    server_channel->setCallback([=]()
                                { return newConnection(server_sock); });
}

Server::~Server() {}

void Server::handleReadEvent(int fd)
{
    char buffer[READ_BUFFER];
    while (true)
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t read_code = read(fd, &buffer, sizeof(buffer));
        if (read_code > 0)
        {
            std::cout << "Received " << buffer << " from fd " << fd << std::endl;
            ssize_t write_code = write(fd, &buffer, sizeof(buffer));

            if (write_code == -1)
            {
                errif(true, "Error: write failed!");
                break;
            }
        }
        else if (read_code == 0)
        {
            std::cout << "Client fd " << fd << " disconnected!" << std::endl;
            close(fd);
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
            close(fd);
        }
    }
}

void Server::newConnection(Socket *serv_sock)
{
    InetAddress *client_addr = new InetAddress();
    Socket *client_sock = new Socket(serv_sock->accept(client_addr));
    client_sock->setnonblocking();
    Channel *client_channel = new Channel(loop, client_sock->getFd());
    client_channel->enableReading();
    std::cout << "connected fd " << client_sock->getFd() << ", addr " << inet_ntoa(client_addr->addr.sin_addr) << ", port " << ntohs(client_addr->addr.sin_port) << std::endl;
    client_channel->setCallback([=]()
                                { return handleReadEvent(client_channel->getFd()); });
}