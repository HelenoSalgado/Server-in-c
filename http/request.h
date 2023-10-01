/*
* Cabeçalho da fonte request.c
* Recebe, trata e responde aos dados enviados pelo cliente
*/

#ifndef REQUEST
#define REQUEST

#include <stdio.h>
#include <string.h>
#include <unistd.h>

// Variáveis externas usadas pela função request
extern char headers[500],
    buffer[BUFFER_SIZE],
    mimeType[MIMETYPE_SIZE],
    status_msg[STATUS_SIZE];


// A função recebe o método de requisição http e o caminho do recurso desejado
int request(char *method, char *path);

#endif