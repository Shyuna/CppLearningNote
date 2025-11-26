#pragma once
#include <arpa/inet.h>

class InetAddress
{
private:
    sockaddr_in addr;
    socklen_t addr_len;

public:
    InetAddress();
    InetAddress(const char *ip, uint16_t port);
    ~InetAddress();
    void setInetAddr(sockaddr_in _addr, socklen_t _addr_len);
    sockaddr_in getAddr();
    socklen_t getAddr_len();
    char *getIp();
    uint16_t getPort();
};

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