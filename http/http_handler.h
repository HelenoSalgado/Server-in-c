/*
* Cabeçalho para http_handler.c
* Recebe, processa e responde aos dados do cliente.
*/

#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "context.h"

// A função recebe um ponteiro para a struct http_context
void http_handler(http_context *ctx);
void handle_get_request(http_context *ctx);

#endif