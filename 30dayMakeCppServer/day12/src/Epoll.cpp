#include "Epoll.h"
#include "util.h"
#include <unistd.h>
#include <cstring>
#include "Channel.h"

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

void Epoll::updateChannel(Channel *channel)
{
    epoll_event ev = {};
    ev.data.ptr = channel;
    ev.events = channel->getEvents();
    if (channel->getInEpoll())
    {
        errif(epoll_ctl(this->epfd, EPOLL_CTL_MOD, channel->getFd(), &ev) == -1, "Error: epoll moduify failed!");
    }
    else
    {
        errif(epoll_ctl(this->epfd, EPOLL_CTL_ADD, channel->getFd(), &ev) == -1, "Error: epoll add failed!");
        channel->setInEpoll();
    }
}

std::vector<Channel *> Epoll::poll(int timeout)
{
    std::vector<Channel *> chs;
    int ndfs = epoll_wait(epfd, events, MAX_EVENTS, timeout);
    for (int i = 0; i < ndfs; i++)
    {
        Channel *channel = reinterpret_cast<Channel *>(events[i].data.ptr);
        channel->setReady(events[i].events);
        chs.push_back(channel);
    }
    return chs;
}