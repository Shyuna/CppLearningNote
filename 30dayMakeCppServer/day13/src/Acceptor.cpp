#include <iostream>
#include "Acceptor.h"
#include "Socket.h"
#include "Channel.h"

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop)
{
    sock = new Socket();
    sock->setnonblocking();

    InetAddress *addr = new InetAddress("127.0.0.1", 1234);
    sock->bind(addr);
    sock->listen();

    acceptChannel = new Channel(loop, sock->getFd());
    acceptChannel->enableRead();
    std::function<void()> cb = [&]()
    { this->acceptConnection(); };
    acceptChannel->setReadCallback(cb);
    delete addr;
}

Acceptor::~Acceptor()
{
    delete sock;
    delete acceptChannel;
}

void Acceptor::acceptConnection()
{
    InetAddress *client_addr = new InetAddress();
    Socket *client_sock = new Socket(sock->accept(client_addr));
    client_sock->setnonblocking();
    std::cout << "connected fd " << client_sock->getFd() << ", addr " << inet_ntoa(client_addr->getAddr().sin_addr) << ", port " << ntohs(client_addr->getAddr().sin_port) << std::endl;
    newConnectionCallback(client_sock);
    delete client_addr;
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> cb)
{
    newConnectionCallback = cb;
}