/*
* Cabeçalho com funções referentes ao arquivo create.c
*/

#ifndef CREATE_SOCKET
#define CREATE_SOCKET

#include <stdio.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdlib.h>

// Cria o socket de conexões e retorna um descritor de socket (número inteiro);
int createSocket(void);

#endif