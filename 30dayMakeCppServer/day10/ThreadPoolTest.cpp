#include <iostream>
#include <string>
#include "src/ThreadPool.h"
#include <thread>

void print(std::string a, std::string b, std::string c)
{
    std::cout << a + "\n";
    std::cout << b + "\n";
    std::cout << c + "\n";
}

void print()
{
    std::this_thread::sleep_for(std::chrono::microseconds(1));
    std::cout << "Hello, world!\n";
}

int main()
{
    ThreadPool *pool = new ThreadPool(2);
    pool->add([]()
              { print(); });
    pool->add([]()
              { print("1", "2.1", "c"); });
    delete pool;
    return 0;
}