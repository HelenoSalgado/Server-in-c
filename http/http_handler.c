#include "../config.h"
#include "../utils/verify.h"
#include "../methods/get/response.h"
#include "http_handler.h"
#include "../utils/regex.h"

void http_handler(http_context *ctx)
{

    char bufferClient[BUFFER_SIZE];
    ssize_t bytes_read = read(ctx->client_fd, bufferClient, sizeof(bufferClient) - 1);
    if (bytes_read > 0)
    {
        bufferClient[bytes_read] = '\0';
    }
    else
    {
        // Handle error or no data
        return;
    }

    // Extract method and path
    regexPath(bufferClient, ctx->path, sizeof(ctx->path), ctx->method, sizeof(ctx->method));

    // Verify MIME type
    verifyMimeType(ctx->path, ctx->mimeType, sizeof(ctx->mimeType));

    int fd = -1;
    if (strcmp(ctx->method, "GET") == 0)
    {
        fd = httpResponse(ctx->path, ctx->status_msg, sizeof(ctx->status_msg), ctx->mimeType, sizeof(ctx->mimeType));
    }

    if (fd != -1)
    {
        ssize_t sizeFile = read(fd, ctx->buffer, sizeof(ctx->buffer));
        close(fd);

        if (sizeFile >= 0) // read can return 0 for empty file
        {
            snprintf(ctx->headers, sizeof(ctx->headers), "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %zd\r\nConnection: close\r\n\r\n", ctx->status_msg, ctx->mimeType, sizeFile);
            snprintf(ctx->data, sizeof(ctx->data), "%s%s", ctx->headers, ctx->buffer);
        }
    }
}