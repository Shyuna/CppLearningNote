#include "Socket.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <cstring>

InetAddress::InetAddress()
{
    addr_len = sizeof(addr);
    bzero(&addr, addr_len);
}

InetAddress::InetAddress(const char *ip, uint16_t port)
{
    addr_len = sizeof(addr);
    bzero(&addr, addr_len);
    addr.sin_addr.s_addr = inet_addr(ip);
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
}

InetAddress::~InetAddress()
{
}

void InetAddress::setInetAddr(sockaddr_in _addr, socklen_t _addr_len)
{
    addr = _addr;
    addr_len = _addr_len;
}

sockaddr_in InetAddress::getAddr()
{
    return addr;
}

socklen_t InetAddress::getAddr_len()
{
    return addr_len;
}

char *InetAddress::getIp()
{
    return inet_ntoa(addr.sin_addr);
}
uint16_t InetAddress::getPort()
{
    return ntohs(addr.sin_port);
}

Socket::Socket()
{
    fd = socket(AF_INET, SOCK_STREAM, 0);
    errif(fd == -1, "Error: create socket failed!");
}

Socket::Socket(int fd) : fd(fd)
{
    errif(fd == -1, "Error: create socket failed! fd is illegal!");
}

Socket::~Socket()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
    else
    {
        errif(true, "Error: close socket failed! fd is illegal!");
    }
}

int Socket::getFd()
{
    errif(fd == -1, "Error: get fd failed! fd is illegal!");
    return fd;
}

void Socket::bind(InetAddress *addr)
{
    sockaddr_in address = addr->getAddr();
    socklen_t addr_len = addr->getAddr_len();
    errif(-1 == ::bind(fd, reinterpret_cast<const sockaddr *>(&address), addr_len), "Error: bind failed!");
    addr->setInetAddr(address, addr_len);
}

void Socket::listen()
{
    errif(-1 == ::listen(fd, SOMAXCONN), "Error: listen failed!");
}

void Socket::setnonblocking()
{
    fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) | O_NONBLOCK);
}

int Socket::accept(InetAddress *addr)
{
    sockaddr_in address;
    socklen_t addr_len = sizeof(address);
    int client_fd = ::accept(fd, reinterpret_cast<sockaddr *>(&address), &addr_len);
    errif(client_fd == -1, "Error: accept failed!");
    addr->setInetAddr(address, addr_len);
    return client_fd;
}

void Socket::connect(InetAddress *addr)
{
    sockaddr_in serv_addr = addr->getAddr();
    socklen_t len = addr->getAddr_len();
    errif(::connect(fd, (sockaddr *)&serv_addr, len) == -1, "socket connect error");
}