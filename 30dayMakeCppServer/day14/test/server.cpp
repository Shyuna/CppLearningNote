#include "EventLoop.h"
#include "Server.h"
#include "Connection.h"
#include <unistd.h>
#include "util.h"
#include <iostream>
#include <cstring>

#define READ_BUFFER 1024

int main()
{
    std::cout << "Congratulations for the success of final test!" << std::endl;
    EventLoop *loop = new EventLoop();
    Server *server = new Server(loop);
    server->onConnection([](Connection *conn)
    { 
        int fd = conn->getFd();
        char buffer[READ_BUFFER];
        while (true)
        {
            bzero(&buffer, sizeof(buffer));
            ssize_t read_code = read(fd, &buffer, sizeof(buffer));
            if (read_code > 0)
            {
                conn->readCallBack(buffer, READ_BUFFER);
                std::cout << "Received " << buffer << " from fd " << fd << std::endl;
            }
            else if (read_code == 0)
            {
                std::cout << "Client fd " << fd << " disconnected!" << std::endl;
                close(fd);
                break;
            }
            else if ((read_code == -1) && (errno & EAGAIN) | (errno & EWOULDBLOCK)) // 非阻塞模式下，缓冲区无数据可读，返回-1，设置errno
            {
                std::cout << "Read once completed!" << std::endl;
                ssize_t write_code = conn->writeCallBack();

                if (write_code == -1)
                {
                    errif(true, "Error: write failed!");
                    break;
                }
                
                break;
            }
            else if ((read_code == -1) && (errno & EINTR)) // 输入被中断的情况，continue继续读取输入
            {
                std::cout << "Read Interupted!" << std::endl;
                continue;
            }
            else
            {
                std::cout << "Error: something undefined happened!" << std::endl;
                // deleteConnectionCallback(sock);
            }
        } 
    });
    loop->loop();
    delete loop;
    delete server;
    return 0;
}