#include "config.h"
#include "verify.h"
#include "cache.h"
#include "http_handler.h"
#include "regex.h"
#include "logger.h"
#include "path.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <fcntl.h>
#include <pthread.h>

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
    int cache_hit = 0;

    // Primeiro, verifica se o arquivo está no cache
    pthread_mutex_lock(&cache_mutex);
    cache_entry_t *cached_entry = cache_get(&server_cache, ctx->path);
    if (cached_entry) {
        // Cache hit! Usa os dados do cache
        strncpy(ctx->buffer, cached_entry->data, sizeof(ctx->buffer) - 1);
        ctx->buffer[sizeof(ctx->buffer) - 1] = '\0';
        strncpy(ctx->mimeType, cached_entry->mime_type, sizeof(ctx->mimeType) - 1);
        ctx->mimeType[sizeof(ctx->mimeType) - 1] = '\0';
        strncpy(ctx->status_msg, "200 OK", sizeof(ctx->status_msg) - 1);
        ctx->status_msg[sizeof(ctx->status_msg) - 1] = '\0';
        sizeFile = cached_entry->size;
        cache_hit = 1;
    }
    pthread_mutex_unlock(&cache_mutex);

    if (!cache_hit) {
        // Cache miss - precisa ler do disco
        if (sanitize_path(DIR_ROOT, ctx->path, sanitized_path, sizeof(sanitized_path)) == 0) {
            fd = open(sanitized_path, O_RDONLY);
        } else {
            fd = -1; // Garante que fd seja -1 se a sanitização falhar
        }

        if (fd != -1) {
            // Arquivo encontrado e aberto com sucesso
            sizeFile = read(fd, ctx->buffer, sizeof(ctx->buffer) - 1);
            close(fd);
            if (sizeFile < 0) {
                send_error_response(ctx, "500 Internal Server Error", "<html><body><h1>500 Internal Server Error</h1></body></html>");
                return;
            }
            ctx->buffer[sizeFile] = '\0';
            strncpy(ctx->status_msg, "200 OK", sizeof(ctx->status_msg) - 1);
            verifyMimeType(sanitized_path, ctx->mimeType, sizeof(ctx->mimeType));

            // Adiciona ao cache (só arquivos pequenos para evitar pressão de memória)
            if (sizeFile > 0 && sizeFile <= CACHE_MAX_FILE_SIZE) {
                pthread_mutex_lock(&cache_mutex);
                cache_put(&server_cache, ctx->path, ctx->buffer, sizeFile, ctx->mimeType);
                pthread_mutex_unlock(&cache_mutex);
            }
        } else {
            // Arquivo não encontrado ou erro de sanitização, tenta servir 404.html
            char path_404[PATH_MAX];
            snprintf(path_404, sizeof(path_404), "%s%s", DIR_ROOT, PAGE_NOT_FOUND);
            fd = open(path_404, O_RDONLY);
            if (fd != -1) {
                sizeFile = read(fd, ctx->buffer, sizeof(ctx->buffer) - 1);
                close(fd);
                if (sizeFile < 0) {
                    send_error_response(ctx, "500 Internal Server Error", "<html><body><h1>500 Internal Server Error</h1></body></html>");
                    return;
                }
                ctx->buffer[sizeFile] = '\0';
                strncpy(ctx->status_msg, "404 Not Found", sizeof(ctx->status_msg) - 1);
                strncpy(ctx->mimeType, "text/html", sizeof(ctx->mimeType) - 1);
            } else {
                // Se nem o 404.html for encontrado, envia uma resposta 404 genérica
                send_error_response(ctx, "404 Not Found", "<html><body><h1>404 Not Found</h1></body></html>");
                return;
            }
        }
    }

    ctx->status_msg[sizeof(ctx->status_msg) - 1] = '\0';
    ctx->mimeType[sizeof(ctx->mimeType) - 1] = '\0';

    snprintf(ctx->headers, sizeof(ctx->headers), "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %zd\r\nX-Cache: %s\r\nConnection: close\r\n\r\n", 
             ctx->status_msg, ctx->mimeType, sizeFile, cache_hit ? "HIT" : "MISS");

    if (strcmp(ctx->method, "GET") == 0) {
        snprintf(ctx->data, sizeof(ctx->data), "%s%s", ctx->headers, ctx->buffer);
    } else { // HEAD
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
        write(ctx->client_fd, ctx->data, strlen(ctx->data));
        return;
    }

    if (strcmp(ctx->method, "GET") == 0 || strcmp(ctx->method, "HEAD") == 0) {
        handle_get_request(ctx);
    } else {
        send_error_response(ctx, "501 Not Implemented", "<html><body><h1>501 Not Implemented</h1></body></html>");
    }
    
    // Envia a resposta
    write(ctx->client_fd, ctx->data, strlen(ctx->data));
    
    // Registra a requisição no log
    log_request(ctx);
}
