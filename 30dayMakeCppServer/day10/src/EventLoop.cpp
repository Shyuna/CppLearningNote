#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"
#include "ThreadPool.h"
#include <vector>

EventLoop::EventLoop()
{
    ep = new Epoll();
    threadPool = new ThreadPool();
}

EventLoop::~EventLoop()
{
    delete ep;
    delete threadPool;
}

void EventLoop::loop()
{
    while (!quit)
    {
        std::vector<Channel *> channels = ep->poll();
        for (Channel *channel : channels)
        {
            channel->handleEvent();
        }
    }
}

void EventLoop::updateChannel(Channel *channel)
{
    ep->updateChannel(channel);
}

void EventLoop::addThread(std::function<void()> func)
{
    threadPool->add(func);
}