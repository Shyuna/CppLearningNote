#include <functional>
#include <cstring>
#include <functional>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Server.h"
#include "Socket.h"
#include "Acceptor.h"
#include "Connection.h"
#include "Channel.h"

#define READ_BUFFER 1024

Server::Server(EventLoop *loop) : loop(loop)
{
    acceptor = new Acceptor(loop);
    acceptor->setNewConnectionCallback([=](Socket *sock)
                                       { newConnection(sock); });
}

Server::~Server()
{
    delete acceptor;
}

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

void Server::newConnection(Socket *client_sock)
{
    Connection *client_connect = new Connection(loop, client_sock);
    connections.insert({client_sock->getFd(), client_connect});
    client_connect->setDeleteConnectionCallback([=](Socket *sock)
                                                { deleteConnection(sock); });
}

void Server::deleteConnection(Socket *sock)
{
    delete connections[sock->getFd()];
    connections.erase(sock->getFd());
}