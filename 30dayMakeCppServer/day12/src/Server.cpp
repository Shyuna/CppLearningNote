#include <functional>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "util.h"
#include "Epoll.h"
#include "Server.h"
#include "Socket.h"
#include "Acceptor.h"
#include "Connection.h"
#include "Channel.h"
#include "ThreadPool.h"
#include "EventLoop.h"

#define READ_BUFFER 1024

Server::Server(EventLoop *loop) : mainReactor(loop)
{
    acceptor = new Acceptor(loop);
    acceptor->setNewConnectionCallback([=](Socket *sock)
                                       { newConnection(sock); });

    int pool_size = std::thread::hardware_concurrency();
    thpool = new ThreadPool(pool_size);
    for (int i = 0; i < pool_size; i++)
    {
        subReactors.emplace_back(new EventLoop());
    }

    for (int i = 0; i < pool_size; i++)
    {
        thpool->add(&EventLoop::loop, subReactors[i]);
    }
}

Server::~Server()
{
    delete acceptor;
    delete thpool;
}

void Server::newConnection(Socket *client_sock)
{
    int rand = client_sock->getFd() % subReactors.size();
    Connection *client_connect = new Connection(subReactors[rand], client_sock);
    connections.insert({client_sock->getFd(), client_connect});
    client_connect->setDeleteConnectionCallback([=](int sockfd)
                                                { deleteConnection(sockfd); });

    connections[client_sock->getFd()] = client_connect;
}

void Server::deleteConnection(int sockfd)
{
    delete connections[sockfd];
    connections.erase(sockfd);
}