#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>
#include <unistd.h>
#include "util.h"

int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_add;
    serv_add.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_add.sin_family = AF_INET;
    serv_add.sin_port = htons(8888);

    bind(socket_fd, reinterpret_cast<const sockaddr *>(&serv_add), sizeof(serv_add));

    listen(socket_fd, SOMAXCONN);

    sockaddr_in client_add;
    socklen_t clinet_len = sizeof(client_add);
    bzero(&client_add, clinet_len);

    int client_fd = accept(socket_fd, reinterpret_cast<sockaddr *>(&client_add), &clinet_len);

    while (true)
    {
        char buffer[1024];
        bzero(&buffer, sizeof(buffer));

        ssize_t read_code = read(client_fd, &buffer, sizeof(buffer));
        if (read_code > 0)
        {
            std::cout << "Received " << buffer << " from fd " << client_fd << std::endl;
            ssize_t write_code = write(client_fd, &buffer, sizeof(buffer));

            if (write_code == -1)
            {
                errif(true, "Error: write failed!");
                break;
            }
        }
        else if (read_code == 0)
        {
            std::cout << "Error: client disconnected!" << std::endl;
            break;
        }
        else
        {
            std::cout << "Error: read failed!" << std::endl;
            break;
        }
    }
    close(client_fd);
    close(socket_fd);
    return 0;
}
