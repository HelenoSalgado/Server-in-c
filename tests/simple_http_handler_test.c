#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h> // For exit()
#include <fcntl.h>  // For open() constants

#include "../http/http_handler.h"
#include "../http/context.h"
#include "../config.h"
#include "../utils/logger.h" // Added for LogLevel

// Códigos de cor ANSI
#define VERDE "\033[32m"
#define VERMELHO "\033[31m"
#define AMARELO "\033[33m"
#define AZUL "\033[34m"
#define RESET "\033[0m"

// Forward declaration for the mock file content read function
ssize_t __wrap_read_file_content(int fd, void *buf, size_t count);

// Global variables for mock read (moved before set_mock_request)
const char *__wrap_read_mock_request = NULL;
size_t __wrap_read_mock_request_len = 0;
size_t __wrap_read_mock_read_offset = 0;

// Mock read function for http_handler and file content
ssize_t __wrap_read(int fd, void *buf, size_t count) {
    if (fd == 999) { // Our special client_fd for mocking
        if (__wrap_read_mock_request == NULL) {
            return 0; // No mock request set
        }

        size_t bytes_to_copy = count;
        if (__wrap_read_mock_read_offset + bytes_to_copy > __wrap_read_mock_request_len) {
            bytes_to_copy = __wrap_read_mock_request_len - __wrap_read_mock_read_offset;
        }

        if (bytes_to_copy > 0) {
            memcpy(buf, __wrap_read_mock_request + __wrap_read_mock_read_offset, bytes_to_copy);
            __wrap_read_mock_read_offset += bytes_to_copy;
            return bytes_to_copy;
        } else {
            return 0; // End of mock request
        }
    } else if (fd >= 100 && fd <= 103) { // Dummy FDs for file content
        return __wrap_read_file_content(fd, buf, count);
    }
    // For other FDs, call the real read (if needed, though not in this test scenario)
    // Or, if we don't expect real file reads, return an error.
    // For now, let's return -1 to indicate an unexpected read.
    return -1;
}

// Helper to set the mock request for __wrap_read
void set_mock_request(const char *request) {
    static char request_buffer[BUFFER_SIZE];
    strncpy(request_buffer, request, BUFFER_SIZE - 1);
    request_buffer[BUFFER_SIZE - 1] = '\0';
    __wrap_read_mock_request = request_buffer;
    __wrap_read_mock_request_len = strlen(request_buffer);
    __wrap_read_mock_read_offset = 0;
}

#define ASSERT_EQ(expected, actual, message) do { \
    if ((expected) != (actual)) { \
        printf(VERMELHO "FALHOU: %s - Esperado %d, obtido %d\n" RESET, message, (int)(expected), (int)(actual)); \
        exit(1); \
    } \
} while(0)

#define ASSERT_STR_EQ(expected, actual, message) do { \
    if (strcmp((expected), (actual)) != 0) { \
        printf(VERMELHO "FALHOU: %s - Esperado \"%s\", obtido \"%s\"\n" RESET, message, (expected), (actual)); \
        exit(1); \
    } \
} while(0)

#define ASSERT_HTTP_REQUEST(ctx, exp_method, exp_path, exp_status, test_name) do { \
    ASSERT_STR_EQ(exp_method, ctx.method, test_name ": Método"); \
    ASSERT_STR_EQ(exp_path, ctx.path, test_name ": Caminho"); \
    ASSERT_STR_EQ(exp_status, ctx.status_msg, test_name ": Status"); \
    printf(VERDE "✓ PASSOU: %s\n" RESET, test_name); \
} while(0)

#define ASSERT_RESPONSE_CONTENT(ctx, exp_status_header, exp_content_type, exp_body_present, test_name) do { \
    ASSERT_EQ(strstr(ctx.data, exp_status_header) != NULL, 1, test_name ": Cabeçalho de status"); \
    if (exp_content_type && strlen(exp_content_type) > 0) { \
        ASSERT_EQ(strstr(ctx.data, exp_content_type) != NULL, 1, test_name ": Content-Type"); \
    } \
    if (exp_body_present && strlen(exp_body_present) > 0) { \
        ASSERT_EQ(strstr(ctx.data, exp_body_present) != NULL, 1, test_name ": Conteúdo do corpo"); \
    } \
} while(0)

// Mock for httpResponse to avoid file system interaction
int __wrap_httpResponse(const char *path, char *status_msg, size_t status_msg_size, char *mimeType, size_t mimeType_size) {
    // Simulate success for existing files, failure for non-existent
    if (strcmp(path, "/web/index.html") == 0) {
        strncpy(status_msg, "200 OK", status_msg_size);
        strncpy(mimeType, "text/html", mimeType_size);
        return 100; // Dummy FD
    } else if (strcmp(path, "/web/404.html") == 0) {
        strncpy(status_msg, "404 Not Found", status_msg_size);
        strncpy(mimeType, "text/html", mimeType_size);
        return 101; // Dummy FD
    } else if (strcmp(path, "/web/assets/css/main.css") == 0) {
        strncpy(status_msg, "200 OK", status_msg_size);
        strncpy(mimeType, "text/css", mimeType_size);
        return 102; // Dummy FD
    } else if (strcmp(path, "/web/assets/js/index.js") == 0) {
        strncpy(status_msg, "200 OK", status_msg_size);
        strncpy(mimeType, "application/javascript", mimeType_size);
        return 103; // Dummy FD
    }
    strncpy(status_msg, "404 Not Found", status_msg_size);
    strncpy(mimeType, "text/html", mimeType_size);
    return -1; // Simulate file not found
}

// Mock for sanitize_path to control its behavior
int __wrap_sanitize_path(const char *base_dir, const char *requested_path, char *sanitized_path, size_t sanitized_path_size) {
    if (strcmp(requested_path, "/path_traversal") == 0) {
        return -1; // Simulate path traversal attempt
    } else if (strcmp(requested_path, "/non_existent") == 0) {
        strncpy(sanitized_path, "/web/non_existent", sanitized_path_size);
        sanitized_path[sanitized_path_size - 1] = '\0';
        return 0; // Simulate valid path, but file might not exist
    } else if (strcmp(requested_path, "/") == 0) {
        strncpy(sanitized_path, "/web/index.html", sanitized_path_size);
        sanitized_path[sanitized_path_size - 1] = '\0';
        return 0;
    } else if (strcmp(requested_path, "/index.html") == 0) {
        strncpy(sanitized_path, "/web/index.html", sanitized_path_size);
        sanitized_path[sanitized_path_size - 1] = '\0';
        return 0;
    } else if (strcmp(requested_path, "/assets/css/main.css") == 0) {
        strncpy(sanitized_path, "/web/assets/css/main.css", sanitized_path_size);
        sanitized_path[sanitized_path_size - 1] = '\0';
        return 0;
    } else if (strcmp(requested_path, "/assets/js/index.js") == 0) {
        strncpy(sanitized_path, "/web/assets/js/index.js", sanitized_path_size);
        sanitized_path[sanitized_path_size - 1] = '\0';
        return 0;
    }
    strncpy(sanitized_path, requested_path, sanitized_path_size);
    sanitized_path[sanitized_path_size - 1] = '\0';
    return 0;
}

// Mock for close
int __wrap_close(int fd) {
    return 0; // Always succeed
}

// Mock for read (for file content, not client_fd)
ssize_t __wrap_read_file_content(int fd, void *buf, size_t count) {
    if (fd == 100) { // Dummy FD for /web/index.html
        const char *content = "<html><body><h1>Index</h1></body></html>";
        size_t len = strlen(content);
        if (count < len) len = count;
        memcpy(buf, content, len);
        return len;
    } else if (fd == 101) { // Dummy FD for /web/404.html
        const char *content = "<html><body><h1>404 Not Found</h1></body></html>";
        size_t len = strlen(content);
        if (count < len) len = count;
        memcpy(buf, content, len);
        return len;
    } else if (fd == 102) { // Dummy FD for /web/assets/css/main.css
        const char *content = "body { background-color: #f0f0f0; }\n";
        size_t len = strlen(content);
        if (count < len) len = count;
        memcpy(buf, content, len);
        return len;
    } else if (fd == 103) { // Dummy FD for /web/assets/js/index.js
        const char *content = "console.log('Hello');\n";
        size_t len = strlen(content);
        if (count < len) len = count;
        memcpy(buf, content, len);
        return len;
    }
    return -1; // Simulate read error
}

// Mock for logger_log
void logger_log(int level, const char *format, ...) {
    // Do nothing or print to a mock buffer if needed for testing logger itself
}

// Mock for log_request
void __wrap_log_request(http_context *ctx) {
    // Do nothing for testing
}

// Mock for open
int __wrap_open(const char *pathname, int flags) {
    // Simulate success for existing files, failure for non-existent
    if (strcmp(pathname, "/web/index.html") == 0) {
        return 100; // Dummy FD
    } else if (strcmp(pathname, "/web/404.html") == 0) {
        return 101; // Dummy FD
    } else if (strcmp(pathname, "/web/assets/css/main.css") == 0) {
        return 102; // Dummy FD
    } else if (strcmp(pathname, "/web/assets/js/index.js") == 0) {
        return 103; // Dummy FD
    }
    return -1; // Simulate file not found
}

int main() {
    http_context ctx;

    printf(AZUL "\n-- Executando Testes do Manipulador HTTP --\n" RESET);

    // Test 1: Valid GET request for index.html
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_HTTP_REQUEST(ctx, "GET", "/index.html", "200 OK", "GET index.html");
    ASSERT_RESPONSE_CONTENT(ctx, "HTTP/1.1 200 OK", "Content-Type: text/html", "<html><body><h1>Index</h1></body></html>", "Resposta GET index.html");

    // Test 2: Valid HEAD request for main.css
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("HEAD /assets/css/main.css HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_HTTP_REQUEST(ctx, "HEAD", "/assets/css/main.css", "200 OK", "HEAD arquivo CSS");
    ASSERT_RESPONSE_CONTENT(ctx, "HTTP/1.1 200 OK", "Content-Type: text/css", "", "Resposta HEAD arquivo CSS");
    // For HEAD, body should not be present
    ASSERT_EQ(strstr(ctx.data, "body { background-color: #f0f0f0; }\n") == NULL, 1, "Requisição HEAD: Sem conteúdo do corpo");

    // Test 3: Invalid request format
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("GARBAGE_REQUEST\r\n\r\n");
    http_handler(&ctx);
    ASSERT_HTTP_REQUEST(ctx, "", "", "400 Bad Request", "Formato de requisição inválido");
    ASSERT_RESPONSE_CONTENT(ctx, "HTTP/1.1 400 Bad Request", "", "", "Resposta requisição inválida");

    // Test 4: Unsupported method (POST)
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("POST /submit HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_HTTP_REQUEST(ctx, "POST", "/submit", "501 Not Implemented", "Método POST");
    ASSERT_RESPONSE_CONTENT(ctx, "HTTP/1.1 501 Not Implemented", "", "", "Resposta método POST");

    // Test 5: Path traversal attempt
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("GET /path_traversal HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_HTTP_REQUEST(ctx, "GET", "/path_traversal", "404 Not Found", "Tentativa de travessia de caminho");
    ASSERT_RESPONSE_CONTENT(ctx, "HTTP/1.1 404 Not Found", "", "", "Resposta travessia de caminho");

    // Test 6: Non-existent file
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("GET /non_existent HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_HTTP_REQUEST(ctx, "GET", "/non_existent", "404 Not Found", "Arquivo inexistente");
    ASSERT_RESPONSE_CONTENT(ctx, "HTTP/1.1 404 Not Found", "", "", "Resposta arquivo inexistente");

    // Test 7: Valid GET request for JavaScript file
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("GET /assets/js/index.js HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_HTTP_REQUEST(ctx, "GET", "/assets/js/index.js", "200 OK", "GET arquivo JavaScript");
    ASSERT_RESPONSE_CONTENT(ctx, "HTTP/1.1 200 OK", "Content-Type: application/javascript", "console.log('Hello');", "Resposta GET JavaScript");

    printf(AMARELO "Todos os testes do Manipulador HTTP concluídos.\n" RESET);

    return 0;
}