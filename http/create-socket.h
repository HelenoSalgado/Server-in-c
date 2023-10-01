/*
* Cabeçalho com funções referentes a fonte create-socket.c
*/

#ifndef CREATE_SOCKET
#define CREATE_SOCKET

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

// Cria o socket de conexções e retorna um socket descriptor (número inteiro);
int createSocket(void);

#endif