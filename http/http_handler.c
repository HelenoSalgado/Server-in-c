#include "../config.h"
#include "../utils/verify.h"
#include "../methods/get/response.h"
#include "http_handler.h"
#include "../utils/regex.h"
#include "../utils/logger.h" // Include the logger header
#include <unistd.h> // For read and close
#include <stdio.h> // For fprintf

void http_handler(http_context *ctx)
{
    char bufferClient[BUFFER_SIZE];
    int fd = -1;
    ssize_t sizeFile = 0;
    char method_is_get = 0;
    ssize_t bytes_read = read(ctx->client_fd, bufferClient, sizeof(bufferClient) - 1);
    if (bytes_read <= 0)
    {
        return; // Handle error or no data
    }
    bufferClient[bytes_read] = '\0';

    // Extract method and path
    if (regexPath(bufferClient, ctx->path, sizeof(ctx->path), ctx->method, sizeof(ctx->method)) != 0) {
        fprintf(stderr, "DEBUG: regexPath failed\n");
        strncpy(ctx->status_msg, "400 Bad Request", sizeof(ctx->status_msg) - 1);
        ctx->status_msg[sizeof(ctx->status_msg) - 1] = '\0';
        strncpy(ctx->mimeType, "text/html", sizeof(ctx->mimeType) - 1);
        ctx->mimeType[sizeof(ctx->mimeType) - 1] = '\0';
        const char *body = "<html><body><h1>400 Bad Request</h1></body></html>";
        strncpy(ctx->buffer, body, sizeof(ctx->buffer) - 1);
        ctx->buffer[sizeof(ctx->buffer) - 1] = '\0';
        sizeFile = strlen(ctx->buffer);
        method_is_get = 1; // Treat as GET to send body
        goto send_response; // Skip further processing and go to response sending
    }

    fprintf(stderr, "DEBUG: Method: %s, Path: %s\n", ctx->method, ctx->path);

    // If method is empty, it means the request was malformed (regexPath failed to extract method)
    if (strlen(ctx->method) == 0) {
        fprintf(stderr, "DEBUG: Method is empty\n");
        strncpy(ctx->status_msg, "400 Bad Request", sizeof(ctx->status_msg) - 1);
        ctx->status_msg[sizeof(ctx->status_msg) - 1] = '\0';
        strncpy(ctx->mimeType, "text/html", sizeof(ctx->mimeType) - 1);
        ctx->mimeType[sizeof(ctx->mimeType) - 1] = '\0';
        const char *body = "<html><body><h1>400 Bad Request</h1></body></html>";
        strncpy(ctx->buffer, body, sizeof(ctx->buffer) - 1);
        ctx->buffer[sizeof(ctx->buffer) - 1] = '\0';
        sizeFile = strlen(ctx->buffer);
        method_is_get = 1; // Treat as GET to send body
        goto send_response; // Skip further processing and go to response sending
    }

    if (strcmp(ctx->method, "GET") == 0)
    {
        method_is_get = 1;
        fprintf(stderr, "DEBUG: Calling httpResponse for GET\n");
        fd = httpResponse(ctx->path, ctx->status_msg, sizeof(ctx->status_msg), ctx->mimeType, sizeof(ctx->mimeType));
    }
    else if (strcmp(ctx->method, "HEAD") == 0)
    {
        fprintf(stderr, "DEBUG: Calling httpResponse for HEAD\n");
        fd = httpResponse(ctx->path, ctx->status_msg, sizeof(ctx->status_msg), ctx->mimeType, sizeof(ctx->mimeType));
    }
    else
    {
        // Handle unsupported methods
        fprintf(stderr, "DEBUG: Unsupported method: %s\n", ctx->method);
        strncpy(ctx->status_msg, "501 Not Implemented", sizeof(ctx->status_msg) - 1);
        ctx->status_msg[sizeof(ctx->status_msg) - 1] = '\0';
        strncpy(ctx->mimeType, "text/html", sizeof(ctx->mimeType) - 1);
        ctx->mimeType[sizeof(ctx->mimeType) - 1] = '\0';

        const char *body = "<html><body><h1>501 Not Implemented</h1></body></html>";
        strncpy(ctx->buffer, body, sizeof(ctx->buffer) - 1);
        ctx->buffer[sizeof(ctx->buffer) - 1] = '\0';
        sizeFile = strlen(ctx->buffer);
        method_is_get = 1; // Treat as GET to send body
    }

    if (fd != -1)
    {
        sizeFile = read(fd, ctx->buffer, sizeof(ctx->buffer));
        close(fd);
    }

send_response:
    if (sizeFile >= 0)
    {
        snprintf(ctx->headers, sizeof(ctx->headers), "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %zd\r\nConnection: close\r\n\r\n", ctx->status_msg, ctx->mimeType, sizeFile);
        
        if (method_is_get)
        {
            snprintf(ctx->data, sizeof(ctx->data), "%s%s", ctx->headers, ctx->buffer);
        }
        else
        {   // For HEAD request, only send headers
            snprintf(ctx->data, sizeof(ctx->data), "%s", ctx->headers);
        }
    }

    #ifndef TEST_BUILD
    log_request(ctx); // Log the request details
    #endif
}