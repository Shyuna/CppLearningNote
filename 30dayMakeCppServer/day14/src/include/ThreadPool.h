#pragma once
#include <functional>
#include <vector>
#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <future>
#include <iostream>

class ThreadPool
{
private:
    std::vector<std::thread> threads;
    std::queue<std::function<void()>> tasks;
    std::mutex tasks_mtx;
    std::condition_variable cv;
    bool stop;

public:
    ThreadPool(int size = 10);
    ~ThreadPool();

    template <typename F, typename... Args>
    auto add(F &&, Args &&...) -> std::future<typename std::result_of<F(Args...)>::type>;
};

template <typename F, typename... Args>
auto ThreadPool::add(F &&f, Args &&...args) -> std::future<typename std::result_of<F(Args...)>::type>
{
    using return_type = typename std::result_of<F(Args...)>::type;
    auto task = std::make_shared<std::packaged_task<return_type()>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));

    {
        std::unique_lock<std::mutex> lock(tasks_mtx);

        if (stop)
        {
            std::cout << "thread pool is stoped, cannot add task!" << std::endl;
        }
        else
        {
            tasks.emplace([task]()
                          { (*task)(); });
        }
    }
    cv.notify_one();
    return task->get_future();
}