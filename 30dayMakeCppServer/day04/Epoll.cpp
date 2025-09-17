#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <cstring>

#define MAX_EVENTS 1000

Epoll::Epoll()
{
    epfd = epoll_create1(0);
    errif(epfd == -1, "Error: creat epoll failed!");
    events = new epoll_event[MAX_EVENTS];
    bzero(events, sizeof(*events) * MAX_EVENTS);
}

Epoll::~Epoll()
{
    if (epfd != -1)
    {
        close(epfd);
        epfd = -1;
    }
    delete[] events;
}

void Epoll::addFd(int fd, uint32_t op)
{
    epoll_event ev;
    bzero(&ev, sizeof(ev));
    ev.data.fd = fd;
    ev.events = op;
    errif(epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1, "Error: add epoll failed!");
}

std::vector<epoll_event> Epoll::poll(int timeout)
{
    std::vector<epoll_event> evs;
    int ndfs = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    for (int i = 0; i < ndfs; i++)
    {
        evs.push_back(events[i]);
    }
    return evs;
}