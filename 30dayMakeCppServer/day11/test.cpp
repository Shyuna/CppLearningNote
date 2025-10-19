#include <iostream>
#include <unistd.h>
#include <string.h>
#include <functional>
#include <string>
#include "src/util.h"
#include "src/Buffer.h"
#include "src/InetAddress.h"
#include "src/Socket.h"
#include "src/ThreadPool.h"

void oneClient(int message_cnt, int wait_time)
{
    Socket *sock = new Socket();
    int sockfd = sock->getFd();
    errif(sockfd == -1, "socket create error");

    InetAddress *addr = new InetAddress("127.0.0.1", 8888);

    sock->connect(addr);

    Buffer *readBuffer = new Buffer();
    Buffer *writeBuffer = new Buffer();

    for (int i = 0; i < message_cnt; i++)
    {
        std::string sentString="client fd "+std::to_string(sockfd)+" sent message "+std::to_string(i+1)+"\n";
        writeBuffer->setBuf(sentString);
        std::this_thread::sleep_for(std::chrono::microseconds(wait_time));
        ssize_t write_bytes = write(sockfd, writeBuffer->c_str(), writeBuffer->size());
        if (write_bytes == -1)
        {
            printf("socket already disconnected, can't write any more!\n");
            break;
        }

        char buf[1024];

        while (true)
        {
            bzero(&buf, sizeof(buf));
            ssize_t cur_bytes = read(sockfd, buf, sizeof(buf));

            if (cur_bytes > 0)
            {
                readBuffer->append(buf, cur_bytes);
            }
            if (cur_bytes == 0)
            {
                printf("server socket disconnected!\n");
                exit(0);
            }
            if (readBuffer->size() >= writeBuffer->size())
            {
                printf("message from server: %s\n", writeBuffer->c_str());
                break;
            }
            if (cur_bytes == -1)
            {
                close(sockfd);
                errif(true, "socket read error");
            }
        }
        readBuffer->clear();
    }
    close(sockfd);
    delete sock;
    delete addr;
}

int main(int argc, char *argv[])
{
    int thread_cnt = 10;
    int message_cnt = 10;
    int wait_time = 1;

    int opt;
    while ((opt = getopt(argc, argv, "t:m:w:") != EOF))
    {
        switch (opt)
        {
        case 't':
            thread_cnt = std::stoi(optarg);
            break;
        case 'm':
            message_cnt = std::stoi(optarg);
            break;
        case 'w':
            wait_time = std::stoi(optarg);
            break;
        case '?':
            std::cout << "invalid parameter " << std::endl;
            break;
        default:
            std::cout << "undefined parameter " << std::endl;
            break;
        }
    }

    ThreadPool *pool = new ThreadPool();
    for (int i = 0; i < thread_cnt; i++)
    {
        pool->add([=]()
                  { oneClient(message_cnt, wait_time); });
    }
    delete pool;
    return 0;
}