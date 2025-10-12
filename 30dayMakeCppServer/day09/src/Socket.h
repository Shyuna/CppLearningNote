#pragma once

class InetAddress;
class Socket
{
private:
    int fd = -1;

public:
    Socket();
    Socket(int);
    ~Socket();

    void bind(InetAddress *);
    void listen();
    void setnonblocking();

    int accept(InetAddress *);
    void connect(InetAddress *);

    int getFd();
};
