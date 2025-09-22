#include "Socket.h"
#include "InetAddress.h"
#include "util.h"
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>

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
    errif(-1 == ::bind(fd, reinterpret_cast<const sockaddr *>(&addr->addr), addr->addr_len), "Error: bind failed!");
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
    int client_fd = ::accept(fd, reinterpret_cast<sockaddr *>(&addr->addr), &addr->addr_len);
    errif(client_fd == -1, "Error: accept failed!");
    return client_fd;
}