#include "Channel.h"
#include <sys/epoll.h>
#include <unistd.h>
#include "EventLoop.h"

Channel::Channel(EventLoop *loop, int fd) : loop(loop), fd(fd), events(0), ready(0), inEpoll(false)
{
}

Channel::~Channel()
{
    if (fd != -1)
    {
        close(fd);
        fd = -1;
    }
}

void Channel::handleEvent()
{
    if (ready & (EPOLLIN | EPOLLPRI))
    {
        if (useThreadPool)
        {
            loop->addThread(readCallback);
        }
        else
        {
            readCallback();
        }
    }
    else
    {
        if (useThreadPool)
        {
            loop->addThread(writeCallback);
        }
        else
        {
            writeCallback();
        }
    }
}

void Channel::enableRead()
{
    events = EPOLLIN;
    loop->updateChannel(this);
}

int Channel::getFd()
{
    return fd;
}

uint32_t Channel::getEvents()
{
    return events;
}
uint32_t Channel::getReady()
{
    return ready;
}

bool Channel::getInEpoll()
{
    return inEpoll;
}

void Channel::setInEpoll(bool inpool)
{
    inEpoll = inpool;
}

void Channel::useET()
{
    events |= EPOLLET;
    loop->updateChannel(this);
}

void Channel::setReadCallback(std::function<void()> cb)
{
    readCallback = cb;
}

void Channel::setReady(uint32_t _ev)
{
    ready = _ev;
}

void Channel::setUseThreadPool(bool use)
{
    useThreadPool = use;
}