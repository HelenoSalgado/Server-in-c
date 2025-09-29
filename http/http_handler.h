/*
* Header for http_handler.c
* Receives, processes, and responds to client data.
*/

#ifndef HTTP_HANDLER_H
#define HTTP_HANDLER_H

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "context.h"

// The function takes a pointer to the http_context struct
void http_handler(http_context *ctx);

#endif