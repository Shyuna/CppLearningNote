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
    std::function<void(Connection *)> on_connection_call_back_;

public:
    Server(EventLoop *);
    ~Server();
    void newConnection(Socket *sock);
    void deleteConnection(int sockfd);
    void onConnection(std::function<void(Connection*)> fn);
};
