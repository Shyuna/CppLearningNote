#pragma once

#include <map>
#include <vector>
#include <thread>
class EventLoop;
class Socket;
class Acceptor;
class Connection;
class ThreadPool;
class Server
{
private:
    EventLoop *mainReactor;
    Acceptor *acceptor;
    std::map<int, Connection *> connections;
    ThreadPool *thpool;
    std::vector<EventLoop *> subReactors;

public:
    Server(EventLoop *);
    ~Server();
    void newConnection(Socket *sock);
    void deleteConnection(int sockfd);
};
