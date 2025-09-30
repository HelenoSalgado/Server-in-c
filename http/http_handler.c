#include "../config.h"
#include "../utils/verify.h"
#include "http_handler.h"
#include "../utils/regex.h"
#include "../utils/logger.h"
#include "../utils/path.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>

// Função auxiliar para enviar respostas de erro
static void send_error_response(http_context *ctx, const char *status_code, const char *body) {
    strncpy(ctx->status_msg, status_code, sizeof(ctx->status_msg) - 1);
    ctx->status_msg[sizeof(ctx->status_msg) - 1] = '\0';
    strncpy(ctx->mimeType, "text/html", sizeof(ctx->mimeType) - 1);
    ctx->mimeType[sizeof(ctx->mimeType) - 1] = '\0';
    strncpy(ctx->buffer, body, sizeof(ctx->buffer) - 1);
    ctx->buffer[sizeof(ctx->buffer) - 1] = '\0';
    ssize_t sizeFile = strlen(ctx->buffer);

    snprintf(ctx->headers, sizeof(ctx->headers), "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %zd\r\nConnection: close\r\n\r\n", ctx->status_msg, ctx->mimeType, sizeFile);
    snprintf(ctx->data, sizeof(ctx->data), "%s%s", ctx->headers, ctx->buffer);
}

void handle_get_request(http_context *ctx) {
    char sanitized_path[PATH_MAX];
    int fd = -1;
    ssize_t sizeFile = 0;

    int sanitize_result = sanitize_path(DIR_ROOT, ctx->path, sanitized_path, sizeof(sanitized_path));

    if (sanitize_result != 0) {
        // Tenta servir página de erro 404 personalizada
        fd = open(PAGE_NOT_FOUND, O_RDONLY);
        if (fd != -1) {
            strncpy(ctx->status_msg, "404 Not Found", sizeof(ctx->status_msg) - 1);
            ctx->status_msg[sizeof(ctx->status_msg) - 1] = '\0';
            strncpy(ctx->mimeType, "text/html", sizeof(ctx->mimeType) - 1);
            ctx->mimeType[sizeof(ctx->mimeType) - 1] = '\0';
        } else {
            send_error_response(ctx, "404 Not Found", "<html><body><h1>404 Not Found</h1></body></html>");
            return;
        }
    } else {
        fd = open(sanitized_path, O_RDONLY);
        if (fd != -1) {
            strncpy(ctx->status_msg, "200 OK", sizeof(ctx->status_msg) - 1);
            ctx->status_msg[sizeof(ctx->status_msg) - 1] = '\0';
            verifyMimeType(sanitized_path, ctx->mimeType, sizeof(ctx->mimeType));
        } else {
            // Tenta servir página de erro 404 personalizada
            fd = open(PAGE_NOT_FOUND, O_RDONLY);
            if (fd != -1) {
                strncpy(ctx->status_msg, "404 Not Found", sizeof(ctx->status_msg) - 1);
                ctx->status_msg[sizeof(ctx->status_msg) - 1] = '\0';
                strncpy(ctx->mimeType, "text/html", sizeof(ctx->mimeType) - 1);
                ctx->mimeType[sizeof(ctx->mimeType) - 1] = '\0';
            } else {
                send_error_response(ctx, "404 Not Found", "<html><body><h1>404 Not Found</h1></body></html>");
                return;
            }
        }
    }

    sizeFile = read(fd, ctx->buffer, sizeof(ctx->buffer) - 1);
    close(fd);
    
    if (sizeFile < 0) {
        send_error_response(ctx, "500 Internal Server Error", "<html><body><h1>500 Internal Server Error</h1></body></html>");
        return;
    }
    
    ctx->buffer[sizeFile] = '\0';

    snprintf(ctx->headers, sizeof(ctx->headers), "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %zd\r\nConnection: close\r\n\r\n", ctx->status_msg, ctx->mimeType, sizeFile);

    if (strcmp(ctx->method, "GET") == 0) {
        snprintf(ctx->data, sizeof(ctx->data), "%s%s", ctx->headers, ctx->buffer);
    } else {
        snprintf(ctx->data, sizeof(ctx->data), "%s", ctx->headers);
    }
}

void http_handler(http_context *ctx)
{
    char bufferClient[BUFFER_SIZE];
    ssize_t bytes_read = read(ctx->client_fd, bufferClient, sizeof(bufferClient) - 1);

    if (bytes_read <= 0) {
        return;
    }
    bufferClient[bytes_read] = '\0';

    if (regexPath(bufferClient, ctx->path, sizeof(ctx->path), ctx->method, sizeof(ctx->method)) != 0 || strlen(ctx->method) == 0) {
        send_error_response(ctx, "400 Bad Request", "<html><body><h1>400 Bad Request</h1></body></html>");
        return;
    }

    if (strcmp(ctx->method, "GET") == 0 || strcmp(ctx->method, "HEAD") == 0) {
        handle_get_request(ctx);
    } else {
        send_error_response(ctx, "501 Not Implemented", "<html><body><h1>501 Not Implemented</h1></body></html>");
    }
    
    // Registra a requisição no log
    log_request(ctx);
}
