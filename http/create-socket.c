#include "./../config.h"
#include "create-socket.h"

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

int createSocket(){

    int socket_descriptor;

    struct addrinfo hints, *server;

    memset(&hints, 0, sizeof hints);

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    getaddrinfo(NULL, PORT, &hints, &server);

    socket_descriptor = socket(
        server->ai_family,
        server->ai_socktype, //| SOCK_NONBLOCK,
        server->ai_protocol);

    if (socket_descriptor == -1)
    {
        perror("socket");
        exit(1);
    }

    if (bind(socket_descriptor, server->ai_addr, server->ai_addrlen));

    listen(socket_descriptor, 3);

    printf("%s\n", "Server running...");

    return socket_descriptor;
}