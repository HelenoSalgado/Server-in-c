#ifndef CONTEXT_H
#define CONTEXT_H

#include "config.h"
#include <arpa/inet.h> // For INET_ADDRSTRLEN and INET6_ADDRSTRLEN

/**
 * @brief Holds all the information related to a single HTTP request.
 * This struct is passed between functions to avoid using global variables,
 * making the server more modular and prepared for future concurrency.
 */

typedef struct
{
    int client_fd;
    char client_ip[INET6_ADDRSTRLEN]; // Changed to INET6_ADDRSTRLEN for IPv6 support
    char path[255];
    char method[8];
    char headers[500];
    char buffer[BUFFER_SIZE];
    char data[BUFFER_OUTPUT_SIZE];
    char mimeType[MIMETYPE_SIZE];
    char status_msg[STATUS_SIZE];
} http_context;

#endif
