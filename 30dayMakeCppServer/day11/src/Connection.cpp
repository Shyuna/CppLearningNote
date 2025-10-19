#include <cstring>
#include <iostream>
#include <unistd.h>
#include "Connection.h"
#include "Socket.h"
#include "Buffer.h"
#include "Channel.h"
#include "util.h"

#define READ_BUFFER 1024

Connection::Connection(EventLoop *_loop, Socket *_sock) : loop(_loop), sock(_sock)
{
    sock->setnonblocking();
    channel = new Channel(loop, sock->getFd());
    channel->enableRead();
    channel->setReadCallback([=]()
                         { return echo(channel->getFd()); });
    channel->setUseThreadPool(true);
    channel->useET();
    readBuffer = new Buffer();
}

Connection::~Connection()
{
    deleteConnectionCallback(sock->getFd());
    close(sock->getFd());
    delete sock;
    delete channel;
    delete readBuffer;
}

void Connection::echo(int fd)
{
    char buffer[READ_BUFFER];
    while (true)
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t read_code = read(fd, &buffer, sizeof(buffer));
        if (read_code > 0)
        {
            readBuffer->append(buffer, READ_BUFFER);
            std::cout << "Received " << buffer << " from fd " << fd << std::endl;
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
            ssize_t write_code = write(fd, readBuffer->c_str(), readBuffer->size());

            if (write_code == -1)
            {
                errif(true, "Error: write failed!");
                break;
            }
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
            // deleteConnectionCallback(sock);
        }
    }
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> cb)
{
    deleteConnectionCallback = cb;
}
