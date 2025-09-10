#include <thread>
#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>
#include <deque>


std::mutex mtx_a;
std::condition_variable cv_a;
std::deque<std::string> a;

void t1() {
    {
        std::unique_lock<std::mutex> lk(mtx_a);
        a.push_back("重要信息1");
        cv_a.notify_one();
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::unique_lock<std::mutex> lk(mtx_a);
        a.push_back("重要信息2");
        cv_a.notify_one();
    }
    std::this_thread::sleep_for(std::chrono::seconds(1));
    {
        std::unique_lock<std::mutex> lk(mtx_a);
        a.push_back("重要信息3");
        cv_a.notify_one();
    }
    {
        std::unique_lock<std::mutex> lk(mtx_a);
        a.push_back("结束消息");
        cv_a.notify_all();
    }

}

void t2() {
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::unique_lock<std::mutex> lk(mtx_a);


    while (true) {
        cv_a.wait(lk, [] { return !a.empty(); });
        if (a.front() == "结束消息") { break; }
        std::cout << "线程2收到了" << a.front() << std::endl;
        a.pop_front();
    }
}

void t3() {
    std::unique_lock<std::mutex> lk(mtx_a);

    while (true) {
        cv_a.wait(lk, [] { return !a.empty(); });
        if (a.front() == "结束消息") { break; }
        std::cout << "线程3收到了" << a.front() << std::endl;
        a.pop_front();
    }
}

int main() {
    std::vector<std::thread> pool;
    pool.push_back(std::thread(t2));
    pool.push_back(std::thread(t1));
    pool.push_back(std::thread(t3));

    for (auto &t: pool) {
        t.join();
    }
    return 0;
}


