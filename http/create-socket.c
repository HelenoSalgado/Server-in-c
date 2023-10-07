#include "./../config.h"
#include "create-socket.h"

int createSocket(){

    int sock_des, bind_sock;

    struct sockaddr_in saddr = {
        .sin_family = AF_INET,
        .sin_addr.s_addr = htonl(INADDR_ANY),
        .sin_port = htons(PORT)
    };

    sock_des = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_des == -1)
    {
        perror("socket");
        exit(1);
    }

    bind_sock = bind(sock_des, (struct sockaddr *) &saddr, sizeof(saddr));

    if(bind_sock == -1)
    {
        //char res;
        perror("bind");
        // printf("%s\n", "Você deseja liberar processo anterior e reiniciar? (Y | N)");
        // res = getchar();
        // if(res == "Y" | "y"){
        // system(" kill $(lsof -i tcp | grep -i [0-9] | cut -d' ' -f3)");
        //}
        exit(1);
    }

    listen(sock_des, CONNECTIONS);

    return sock_des;
}