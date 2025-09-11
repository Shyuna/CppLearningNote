#include <sys/socket.h>
#include <arpa/inet.h>
#include <cstring>

int main()
{
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_add;
    serv_add.sin_family = AF_INET;
    serv_add.sin_addr.s_addr = inet_addr("127.0.0.1");
    serv_add.sin_port = htons(8888);

    connect(socket_fd, reinterpret_cast<const sockaddr *>(&serv_add), sizeof(serv_add));
    return 0;
}