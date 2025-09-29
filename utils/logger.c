#include "logger.h"
#include <stdio.h>
#include <time.h>
#include <string.h>
#include <stdlib.h> // Para atoi

FILE *log_file = NULL;

void init_logger(int is_daemon) {
    if (is_daemon) {
        log_file = fopen("server.log", "a");
        if (log_file == NULL) {
            perror("Failed to open log file");
        }
    }
}

void log_request(http_context *ctx) {
    time_t now;
    time(&now);
    char time_str[sizeof("2024-01-01T12:00:00Z")];
    strftime(time_str, sizeof(time_str), "%Y-%m-%dT%H:%M:%SZ", gmtime(&now));

    char status_code_str[4];
    strncpy(status_code_str, ctx->status_msg, 3);
    status_code_str[3] = '\0';
    int status_code = atoi(status_code_str);

    const char *color_start = "\x1b[0m"; // Reset color
    if (status_code >= 200 && status_code < 300) {
        color_start = "\x1b[32m"; // Green para 2xx
    } else if (status_code >= 400 && status_code < 500) {
        color_start = "\x1b[33m"; // Yellow para 4xx
    } else if (status_code >= 500 && status_code < 600) {
        color_start = "\x1b[31m"; // Red para 5xx
    }

    if (log_file != NULL) {
        fprintf(log_file, "%s - %s - \"%s %s\" %s%s\x1b[0m\n",
                time_str,
                ctx->client_ip,
                ctx->method,
                ctx->path,
                color_start,
                ctx->status_msg);
        fflush(log_file);
    } else {
        printf("%s - %s - \"%s %s\" %s%s\x1b[0m\n",
               time_str,
               ctx->client_ip,
               ctx->method,
               ctx->path,
               color_start,
               ctx->status_msg);
    }
}
