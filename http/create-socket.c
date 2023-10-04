#include "./../config.h"
#include "create-socket.h"

int createSocket(){

    int socket_descriptor;

    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(PORT)
    };

    socket_descriptor = socket(AF_INET, SOCK_STREAM, 0);

    if (socket_descriptor == -1)
    {
        perror("socket");
        exit(1);
    }

    bind(socket_descriptor, (struct sockaddr *) &saddr, sizeof(saddr));

    listen(socket_descriptor, CONNECTIONS);

    return socket_descriptor;
}