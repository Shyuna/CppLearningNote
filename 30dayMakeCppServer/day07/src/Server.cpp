#include <cstring>
#include <functional>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "util.h"
#include "Epoll.h"
#include "InetAddress.h"
#include "Socket.h"
#include "Channel.h"
#include "Acceptor.h"
#include "Server.h"

#define READ_BUFFER 1024

Server::Server(EventLoop *loop) : loop(loop)
{
    Acceptor* acceptor=new Acceptor(loop);
    acceptor->setNewConnectionCallback([=](Socket *serv_sock){return newConnection(serv_sock);});
}

Server::~Server() {}

void Server::handleReadEvent(int fd)
{
    char buffer[READ_BUFFER];
    while (true)
    {
        bzero(&buffer, sizeof(buffer));
        ssize_t read_code = read(fd, &buffer, sizeof(buffer));
        if (read_code > 0)
        {
            std::cout << "Received " << buffer << " from fd " << fd << std::endl;
            ssize_t write_code = write(fd, &buffer, sizeof(buffer));

            if (write_code == -1)
            {
                errif(true, "Error: write failed!");
                break;
            }
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
            close(fd);
        }
    }
}

void Server::newConnection(Socket *serv_sock)
{
    InetAddress *client_addr = new InetAddress();
    Socket *client_sock = new Socket(serv_sock->accept(client_addr));
    client_sock->setnonblocking();
    Channel *client_channel = new Channel(loop, client_sock->getFd());
    client_channel->enableReading();
    std::cout << "connected fd " << client_sock->getFd() << ", addr " << inet_ntoa(client_addr->addr.sin_addr) << ", port " << ntohs(client_addr->addr.sin_port) << std::endl;
    client_channel->setCallback([=]()
                                { return handleReadEvent(client_channel->getFd()); });
}

// Server::Server(EventLoop *_loop) : loop(_loop), acceptor(nullptr){ 
//     acceptor = new Acceptor(loop);
//     std::function<void(Socket*)> cb = std::bind(&Server::newConnection, this, std::placeholders::_1);
//     acceptor->setNewConnectionCallback(cb);
// }

// Server::~Server(){
//     delete acceptor;
// }

// void Server::handleReadEvent(int sockfd){
//     char buf[READ_BUFFER];
//     while(true){    //由于使用非阻塞IO，读取客户端buffer，一次读取buf大小数据，直到全部读取完毕
//         bzero(&buf, sizeof(buf));
//         ssize_t bytes_read = read(sockfd, buf, sizeof(buf));
//         if(bytes_read > 0){
//             printf("message from client fd %d: %s\n", sockfd, buf);
//             write(sockfd, buf, sizeof(buf));
//         } else if(bytes_read == -1 && errno == EINTR){  //客户端正常中断、继续读取
//             printf("continue reading");
//             continue;
//         } else if(bytes_read == -1 && ((errno == EAGAIN) || (errno == EWOULDBLOCK))){//非阻塞IO，这个条件表示数据全部读取完毕
//             printf("finish reading once, errno: %d\n", errno);
//             break;
//         } else if(bytes_read == 0){  //EOF，客户端断开连接
//             printf("EOF, client fd %d disconnected\n", sockfd);
//             close(sockfd);   //关闭socket会自动将文件描述符从epoll树上移除
//             break;
//         }
//     }
// }

// void Server::newConnection(Socket *serv_sock){
//     InetAddress *clnt_addr = new InetAddress();      //会发生内存泄露！没有delete
//     Socket *clnt_sock = new Socket(serv_sock->accept(clnt_addr));       //会发生内存泄露！没有delete
//     printf("new client fd %d! IP: %s Port: %d\n", clnt_sock->getFd(), inet_ntoa(clnt_addr->addr.sin_addr), ntohs(clnt_addr->addr.sin_port));
//     clnt_sock->setnonblocking();
//     Channel *clntChannel = new Channel(loop, clnt_sock->getFd());
//     std::function<void()> cb = std::bind(&Server::handleReadEvent, this, clnt_sock->getFd());
//     clntChannel->setCallback(cb);
//     clntChannel->enableReading();
// }