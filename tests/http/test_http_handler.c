#include "../unity/unity.h"
#include "../../http/http_handler.h"
#include "../../http/context.h"
#include "../mocks/mock_syscalls.h"
#include "../mocks/mock_verify.h" // Include mock for verifyMimeType
#include "../../config.h" // For DIR_ROOT, FILE_INDEX, PAGE_NOT_FOUND
#include <string.h>
#include <stdlib.h>

// No local mock for httpResponse anymore

void setUp(void) {
    // Reset mock state before each test
    set_mock_open_return(-1); // Default to file not found
    set_mock_write_return(-1); // Default to successful write (returns count)
    clear_mock_write_buffer();
    set_mock_read_buffer(NULL, 0); // Clear mock read buffer

    // Reset mock realpath state
    set_mock_realpath_fail(); // Default to realpath failing

    // Reset mock verifyMimeType state
    set_mock_verifyMimeType_return(NULL); // Default to no specific mime type
}

void tearDown(void) {
    // Clean up any allocated resources
    clear_mock_write_buffer();
    set_mock_read_buffer(NULL, 0); // Clear mock read buffer
    set_mock_realpath_return(NULL); // Clear any set realpath return value
    clear_mock_realpath_sequence(); // Clear realpath sequence
    set_mock_verifyMimeType_return(NULL); // Clear any set mime type
}

void test_http_handler_valid_get_request(void) {
    http_context ctx;
    memset(&ctx, 0, sizeof(http_context)); // Initialize context
    ctx.client_fd = 1; // Dummy file descriptor

    char request_buffer[] = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    set_mock_read_buffer(request_buffer, strlen(request_buffer));

    // Configure mock realpath for existing files
    // Sequence: realpath(DIR_ROOT + /index.html) -> /abs/path/to/web/index.html
    //           realpath(DIR_ROOT) -> /abs/path/to/web
    const char *realpath_sequence[] = {"/abs/path/to/web/index.html", "/abs/path/to/web"};
    set_mock_realpath_sequence(realpath_sequence, 2);

    // Configure mock open for a successful open of index.html
    set_mock_open_return(100); // Dummy file descriptor

    // Configure mock verifyMimeType
    set_mock_verifyMimeType_return("text/html");

    // Set the content for the file read
    const char* file_content = "<html><body><h1>Hello World!</h1></body></html>";
    set_mock_file_content_buffer(file_content, strlen(file_content));

    http_handler(&ctx);

    // Verify that a 200 OK response was prepared in ctx.data
    TEST_ASSERT_TRUE(strstr(ctx.data, "HTTP/1.1 200 OK") != NULL);
    TEST_ASSERT_TRUE(strstr(ctx.data, "Content-Type: text/html") != NULL);
    TEST_ASSERT_EQUAL_STRING("200 OK", ctx.status_msg);
    TEST_ASSERT_EQUAL_STRING("/index.html", ctx.path);
    TEST_ASSERT_EQUAL_STRING("GET", ctx.method);
}

void test_http_handler_invalid_request(void) {
    http_context ctx;
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 1;

    char request_buffer[] = "INVALID_REQUEST\r\n\r\n"; // Malformed request
    set_mock_read_buffer(request_buffer, strlen(request_buffer));

    http_handler(&ctx);

    // Verify that a 400 Bad Request response was prepared
    TEST_ASSERT_TRUE(strstr(ctx.data, "HTTP/1.1 400 Bad Request") != NULL);
    TEST_ASSERT_EQUAL_STRING("400 Bad Request", ctx.status_msg);
}

void test_http_handler_unsupported_method(void) {
    http_context ctx;
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 1;

    char request_buffer[] = "POST /data HTTP/1.1\r\nHost: localhost\r\n\r\n";
    set_mock_read_buffer(request_buffer, strlen(request_buffer));

    http_handler(&ctx);

    // Verify that a 501 Not Implemented response was prepared
    TEST_ASSERT_TRUE(strstr(ctx.data, "HTTP/1.1 501 Not Implemented") != NULL);
    TEST_ASSERT_EQUAL_STRING("501 Not Implemented", ctx.status_msg);
    TEST_ASSERT_EQUAL_STRING("POST", ctx.method);
}

void test_http_handler_file_not_found(void) {
    http_context ctx;
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 1;

    char request_buffer[] = "GET /nonexistent.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    set_mock_read_buffer(request_buffer, strlen(request_buffer));

    // Configure mock realpath sequence: NULL for original path, then PAGE_NOT_FOUND
    const char *realpath_returns[] = {NULL, PAGE_NOT_FOUND};
    set_mock_realpath_sequence(realpath_returns, 2);

    // Configure mock httpResponse for file not found
    // set_mock_httpResponse_return_fd(101); // Dummy file descriptor for 404.html
    // set_mock_httpResponse_status_and_mime("404 Not Found", "text/html");

    // NEW: Set the content for the 404 page
    const char* not_found_content = "<html><body><h1>404 Not Found</h1></body></html>";
    set_mock_file_content_buffer(not_found_content, strlen(not_found_content));

    http_handler(&ctx);

    // Verify that a 404 Not Found response was prepared
    TEST_ASSERT_TRUE(strstr(ctx.data, "HTTP/1.1 404 Not Found") != NULL);
    TEST_ASSERT_EQUAL_STRING("404 Not Found", ctx.status_msg);
    TEST_ASSERT_EQUAL_STRING(PAGE_NOT_FOUND, ctx.path);
}

int main(void) {
    UNITY_BEGIN();
    printf("\x1b[36m\x1b[3m\nTesting: http_handler function\n\x1b[0m");
    printf("\x1b[36m\x1b[3m  - Expects a 200 OK response for a valid GET request to /index.html\n\x1b[0m");
    RUN_TEST(test_http_handler_valid_get_request);
    printf("\x1b[36m\x1b[3m  - Expects a 400 Bad Request response for an invalid HTTP request\n\x1b[0m");
    RUN_TEST(test_http_handler_invalid_request);
    printf("\x1b[36m\x1b[3m  - Expects a 501 Not Implemented response for an unsupported method (e.g., POST)\n\x1b[0m");
    RUN_TEST(test_http_handler_unsupported_method);
    printf("\x1b[36m\x1b[3m  - Expects a 404 Not Found response when the requested file does not exist\n\x1b[0m");
    RUN_TEST(test_http_handler_file_not_found);
    return UNITY_END();
}

