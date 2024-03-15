#ifndef WEBSERVER
#define WEBSERVER

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>

#include "config.h"
#include "socket/create.h"
#include "utils/regex.h"
#include "http/request.h"

// Variáveis usadas em todo o escopo do programa
char path[255] = {0},
     headers[500], 
     buffer[BUFFER_SIZE], 
     data[BUFFER_OUTPUT_SIZE], 
     mimeType[MIMETYPE_SIZE], 
     status_msg[STATUS_SIZE] = {0};


#endif