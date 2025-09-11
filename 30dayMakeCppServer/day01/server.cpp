#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_add;
    serv_add.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_add.sin_family = AF_INET;
    serv_add.sin_port = htons(8888);

    bind(socket_fd, reinterpret_cast<const sockaddr*>(&serv_add), sizeof(serv_add));

    listen(socket_fd, SOMAXCONN);

    sockaddr_in client_add;
    socklen_t clinet_len = sizeof(client_add);
    bzero(&client_add, clinet_len);

    int client_fd = accept(socket_fd, reinterpret_cast<sockaddr *>(&client_add), &clinet_len);
    std::cout << "fd " << client_fd << std::endl;
    std::cout << "addr " << inet_ntoa(client_add.sin_addr) << std::endl;
    std::cout << "port " << ntohs(client_add.sin_port) << std::endl;
    std::cout << "family " << client_add.sin_family << std::endl;

    return 0;
}