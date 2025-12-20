#pragma once
#include <functional>
#include <string>
class EventLoop;
class Socket;
class Channel;
class Buffer;
class Connection
{
private:
    EventLoop *loop;
    Socket *sock;
    Channel *channel;
    std::function<void(int)> deleteConnectionCallback;
    std::string *inBuffer;
    Buffer *readBuffer;
    std::function<void(Connection *)> on_connect_callback_;

public:
    Connection(EventLoop *_loop, Socket *_sock);
    ~Connection();

    void setDeleteConnectionCallback(std::function<void(int)>);
    void setOnConnectCallback(std::function<void(Connection *)> const &callback);

    void readCallBack(char *, int);
    size_t writeCallBack();
    int getFd();
};
