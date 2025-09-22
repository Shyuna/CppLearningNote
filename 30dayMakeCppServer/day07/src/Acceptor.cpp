#include "Acceptor.h"
#include "Socket.h"
#include "InetAddress.h"
#include "Channel.h"
#include "Server.h"

Acceptor::Acceptor(EventLoop *_loop) : loop(_loop)
{
    sock = new Socket();
    sock->setnonblocking();

    addr = new InetAddress("127.0.0.1", 8888);
    sock->bind(addr);
    sock->listen();

    acceptChannel = new Channel(loop, sock->getFd());
    acceptChannel->enableReading();
    std::function<void()> cb = [&]()
    { this->acceptConnection(); };
    acceptChannel->setCallback(cb);
}

Acceptor::~Acceptor()
{
    delete sock;
    delete addr;
    delete acceptChannel;
}
void Acceptor::acceptConnection()
{
    newConnectionCallback(sock);
}

void Acceptor::setNewConnectionCallback(std::function<void(Socket *)> cb)
{
    newConnectionCallback = cb;
}