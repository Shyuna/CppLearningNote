#include "InetAddress.h"
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