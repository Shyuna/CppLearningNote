#include <iostream>
#include <unistd.h>
#include <cstring>
#include "util.h"
#include "Buffer.h"
#include "Socket.h"

using namespace std;

int main()
{
    Socket *sock = new Socket();
    int sockfd = sock->getFd();
    errif(sockfd == -1, "socket create error");

    InetAddress *addr = new InetAddress("127.0.0.1", 1234);

    sock->connect(addr);

    Buffer *readBuffer = new Buffer();
    Buffer *writeBuffer = new Buffer();

    while (true)
    {
        writeBuffer->getline();
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
    return 0;
}