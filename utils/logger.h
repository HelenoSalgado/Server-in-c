#ifndef LOGGER_H
#define LOGGER_H

#include "../http/context.h"
#include <stdio.h> // Para FILE

extern FILE *log_file;

void init_logger(int is_daemon);

void log_request(http_context *ctx);

#endif
