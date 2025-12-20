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

void Connection::setOnConnectCallback(std::function<void(Connection *)> const &callback)
{
    on_connect_callback_ = callback;
    channel->setReadCallback([this]()
                             { on_connect_callback_(this); });
}

void Connection::setDeleteConnectionCallback(std::function<void(int)> cb)
{
    deleteConnectionCallback = cb;
}

void Connection::readCallBack(char *buffer, int len)
{
    readBuffer->append(buffer, len);
}

size_t Connection::writeCallBack()
{
    ssize_t write_code = write(channel->getFd(), readBuffer->c_str(), readBuffer->size());
    readBuffer->clear();
}

int Connection::getFd()
{
    return channel->getFd();
}
