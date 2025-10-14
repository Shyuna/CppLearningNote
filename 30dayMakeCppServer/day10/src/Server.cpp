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