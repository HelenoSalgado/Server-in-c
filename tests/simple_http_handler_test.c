#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h> // For exit()

#include "../http/http_handler.h"
#include "../http/context.h"
#include "../config.h"
#include "../utils/logger.h" // Added for LogLevel

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
        printf("FAIL: %s - Expected %d, got %d\n", message, (int)(expected), (int)(actual)); \
        exit(1); \
    } \
    printf("PASS: %s\n", message); \
} while(0)

#define ASSERT_STR_EQ(expected, actual, message) do { \
    if (strcmp((expected), (actual)) != 0) { \
        printf("FAIL: %s - Expected \"%s\", got \"%s\"\n", message, (expected), (actual)); \
        exit(1); \
    } \
    printf("PASS: %s\n", message); \
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

int main() {
    http_context ctx;
    // int result; // Removed as it's unused and causes warning

    printf("\n-- Running Simple HTTP Handler Tests --\n");

    // Test 1: Valid GET request for index.html
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999; // Special FD for mock read
    set_mock_request("GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_STR_EQ("GET", ctx.method, "Test 1: Extracted method");
    ASSERT_STR_EQ("/index.html", ctx.path, "Test 1: Extracted path");
    ASSERT_STR_EQ("200 OK", ctx.status_msg, "Test 1: Status message");
    // Check if data contains headers and body
    ASSERT_EQ(strstr(ctx.data, "HTTP/1.1 200 OK") != NULL, 1, "Test 1: Data contains 200 OK header");
    ASSERT_EQ(strstr(ctx.data, "Content-Type: text/html") != NULL, 1, "Test 1: Data contains Content-Type header");
    ASSERT_EQ(strstr(ctx.data, "<html><body><h1>Index</h1></body></html>") != NULL, 1, "Test 1: Data contains body");

    // Test 2: Valid HEAD request for main.css
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("HEAD /assets/css/main.css HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_STR_EQ("HEAD", ctx.method, "Test 2: Extracted method");
    ASSERT_STR_EQ("/assets/css/main.css", ctx.path, "Test 2: Extracted path");
    ASSERT_STR_EQ("200 OK", ctx.status_msg, "Test 2: Status message");
    // For HEAD, only headers should be present, no body
    ASSERT_EQ(strstr(ctx.data, "HTTP/1.1 200 OK") != NULL, 1, "Test 2: Data contains 200 OK header");
    ASSERT_EQ(strstr(ctx.data, "Content-Type: text/css") != NULL, 1, "Test 2: Data contains Content-Type header");
    ASSERT_EQ(strstr(ctx.data, "body { background-color: #f0f0f0; }\n") == NULL, 1, "Test 2: Data does not contain body");

    // Test 3: Invalid request format (regexPath fails)
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("GARBAGE_REQUEST\r\n\r\n");
    http_handler(&ctx);
    ASSERT_STR_EQ("", ctx.method, "Test 3: Empty method"); // regexPath should fail to extract
    ASSERT_STR_EQ("400 Bad Request", ctx.status_msg, "Test 3: Status message");
    ASSERT_EQ(strstr(ctx.data, "HTTP/1.1 400 Bad Request") != NULL, 1, "Test 3: Data contains 400 Bad Request header");

    // Test 4: Unsupported method (e.g., POST)
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("POST /submit HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_STR_EQ("POST", ctx.method, "Test 4: Extracted method");
    ASSERT_STR_EQ("/submit", ctx.path, "Test 4: Extracted path");
    ASSERT_STR_EQ("501 Not Implemented", ctx.status_msg, "Test 4: Status message");
    ASSERT_EQ(strstr(ctx.data, "HTTP/1.1 501 Not Implemented") != NULL, 1, "Test 4: Data contains 501 Not Implemented header");

    // Test 5: Path traversal attempt (sanitize_path fails)
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("GET /path_traversal HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_STR_EQ("GET", ctx.method, "Test 5: Extracted method");
    ASSERT_STR_EQ("/path_traversal", ctx.path, "Test 5: Extracted path");
    ASSERT_STR_EQ("404 Not Found", ctx.status_msg, "Test 5: Status message"); // sanitize_path failure leads to 404
    ASSERT_EQ(strstr(ctx.data, "HTTP/1.1 404 Not Found") != NULL, 1, "Test 5: Data contains 404 Not Found header");

    // Test 6: Non-existent file (httpResponse returns -1)
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999;
    set_mock_request("GET /non_existent HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_STR_EQ("GET", ctx.method, "Test 6: Extracted method");
    ASSERT_STR_EQ("/non_existent", ctx.path, "Test 6: Extracted path");
    ASSERT_STR_EQ("404 Not Found", ctx.status_msg, "Test 6: Status message");
    ASSERT_EQ(strstr(ctx.data, "HTTP/1.1 404 Not Found") != NULL, 1, "Test 6: Data contains 404 Not Found header");

    // Test 7: Valid GET request for index.js
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 999; // Special FD for mock read
    set_mock_request("GET /assets/js/index.js HTTP/1.1\r\nHost: localhost\r\n\r\n");
    http_handler(&ctx);
    ASSERT_STR_EQ("GET", ctx.method, "Test 7: Extracted method");
    ASSERT_STR_EQ("/assets/js/index.js", ctx.path, "Test 7: Extracted path");
    ASSERT_STR_EQ("200 OK", ctx.status_msg, "Test 7: Status message");
    // Check if data contains headers and body
    ASSERT_EQ(strstr(ctx.data, "HTTP/1.1 200 OK") != NULL, 1, "Test 7: Data contains 200 OK header");
    ASSERT_EQ(strstr(ctx.data, "Content-Type: application/javascript") != NULL, 1, "Test 7: Data contains Content-Type header");
    ASSERT_EQ(strstr(ctx.data, "console.log('Hello');") != NULL, 1, "Test 7: Data contains body");

    printf("\nAll Simple HTTP Handler Tests completed.\n");

    return 0;
}