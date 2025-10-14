#include "ThreadPool.h"
#include <iostream>

ThreadPool::ThreadPool(int size)
{
    stop = false;
    for (int i = 0; i < size; i++)
    {
        threads.push_back(std::thread([this](){while (true){
        std::function<void()> task;
        {
            std::unique_lock<std::mutex> lock(tasks_mtx);
            cv.wait(lock, [this](){ return stop || !tasks.empty(); });
            if (stop && tasks.empty())
            {
                break;
            }
            task = tasks.front();
            tasks.pop();
        }
        task();} }));
    }
}

ThreadPool::~ThreadPool()
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);
        stop = true;
    }

    for (auto &thread : threads)
    {
        if (thread.joinable())
        {
            thread.join();
        }
    }
    cv.notify_all();
}

void ThreadPool::add(std::function<void()> task)
{
    {
        std::unique_lock<std::mutex> lock(tasks_mtx);

        if (stop)
        {
            std::cout << "thread pool is stoped, cannot add task!" << std::endl;
        }
        else
        {
            tasks.push(task);
        }
    }
    cv.notify_one();
}