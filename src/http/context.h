#ifndef CONTEXT_H
#define CONTEXT_H

#include "config.h"
#include <arpa/inet.h> // Para INET_ADDRSTRLEN e INET6_ADDRSTRLEN

/**
 * @brief Armazena todas as informações relacionadas a uma única requisição HTTP.
 * Esta struct é passada entre as funções para evitar o uso de variáveis globais,
 * tornando o servidor mais modular e preparado para concorrência futura.
 */
typedef struct
{
    int client_fd;
    char client_ip[INET6_ADDRSTRLEN]; // Suporte para IPv6
    char path[255];
    char method[8];
    char headers[500];
    char buffer[BUFFER_SIZE];
    char data[BUFFER_OUTPUT_SIZE];
    char mimeType[MIMETYPE_SIZE];
    char status_msg[STATUS_SIZE];
} http_context;

#endif
