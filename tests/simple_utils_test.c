#include <stdio.h>
#include <string.h>
#include "../utils/regex.h"
#include "../utils/verify.h"

#define ASSERT_EQ(expected, actual, message) do { \
    if ((expected) != (actual)) { \
        printf("FAIL: %s - Expected %d, got %d\n", message, (int)(expected), (int)(actual)); \
        return 1; \
    } \
    printf("PASS: %s\n", message); \
} while(0)

#define ASSERT_STR_EQ(expected, actual, message) do { \
    if (strcmp((expected), (actual)) != 0) { \
        printf("FAIL: %s - Expected \"%s\", got \"%s\"\n", message, (expected), (actual)); \
        return 1; \
    } \
    printf("PASS: %s\n", message); \
} while(0)

int main() {
    char path[256];
    char method[16];
    char mimeType[64];
    int result;

    // --- Tests for regexPath ---

    // Test 1: Valid GET request
    char request1[] = "GET /index.html HTTP/1.1\r\n";
    result = regexPath(request1, path, sizeof(path), method, sizeof(method));
    ASSERT_EQ(0, result, "regexPath Test 1: Valid GET request");
    ASSERT_STR_EQ("/index.html", path, "regexPath Test 1: Extracted path");
    ASSERT_STR_EQ("GET", method, "regexPath Test 1: Extracted method");

    // Test 2: Valid HEAD request
    char request2[] = "HEAD /data.json HTTP/1.1\r\n";
    result = regexPath(request2, path, sizeof(path), method, sizeof(method));
    ASSERT_EQ(0, result, "regexPath Test 2: Valid HEAD request");
    ASSERT_STR_EQ("/data.json", path, "regexPath Test 2: Extracted path");
    ASSERT_STR_EQ("HEAD", method, "regexPath Test 2: Extracted method");

    // Test 3: Unsupported method
    char request3[] = "POST /submit HTTP/1.1\r\n";
    result = regexPath(request3, path, sizeof(path), method, sizeof(method));
    ASSERT_EQ(0, result, "regexPath Test 3: Unsupported method");
    ASSERT_STR_EQ("/submit", path, "regexPath Test 3: Extracted path");
    ASSERT_STR_EQ("POST", method, "regexPath Test 3: Extracted method");

    // Test 4: Invalid request format
    char request4[] = "INVALID_REQUEST\r\n";
    result = regexPath(request4, path, sizeof(path), method, sizeof(method));
    ASSERT_EQ(-1, result, "regexPath Test 4: Invalid request format");

    printf("\nAll regexPath tests completed.\n");

    // --- Tests for verifyMimeType ---

    // Test 1: HTML file
    result = verifyMimeType("/path/to/index.html", mimeType, sizeof(mimeType));
    ASSERT_EQ(0, result, "verifyMimeType Test 1: HTML file");
    ASSERT_STR_EQ("text/html", mimeType, "verifyMimeType Test 1: MIME type");

    // Test 2: CSS file
    result = verifyMimeType("/path/to/style.css", mimeType, sizeof(mimeType));
    ASSERT_EQ(0, result, "verifyMimeType Test 2: CSS file");
    ASSERT_STR_EQ("text/css", mimeType, "verifyMimeType Test 2: MIME type");

    // Test 3: JavaScript file
    result = verifyMimeType("/path/to/script.js", mimeType, sizeof(mimeType));
    ASSERT_EQ(0, result, "verifyMimeType Test 3: JavaScript file");
    ASSERT_STR_EQ("application/javascript", mimeType, "verifyMimeType Test 3: MIME type");

    // Test 4: PNG image
    result = verifyMimeType("/path/to/image.png", mimeType, sizeof(mimeType));
    ASSERT_EQ(0, result, "verifyMimeType Test 4: PNG file");
    ASSERT_STR_EQ("application/octet-stream", mimeType, "verifyMimeType Test 4: MIME type");

    // Test 5: JPEG image
    result = verifyMimeType("/path/to/image.jpeg", mimeType, sizeof(mimeType));
    ASSERT_EQ(0, result, "verifyMimeType Test 5: JPEG file");
    ASSERT_STR_EQ("application/octet-stream", mimeType, "verifyMimeType Test 5: MIME type");

    // Test 6: JSON file
    result = verifyMimeType("/path/to/data.json", mimeType, sizeof(mimeType));
    ASSERT_EQ(0, result, "verifyMimeType Test 6: JSON file");
    ASSERT_STR_EQ("application/json", mimeType, "verifyMimeType Test 6: JSON file");

    // Test 7: Unknown extension
    result = verifyMimeType("/path/to/document.xyz", mimeType, sizeof(mimeType));
    ASSERT_EQ(0, result, "verifyMimeType Test 7: Unknown extension");
    ASSERT_STR_EQ("application/octet-stream", mimeType, "verifyMimeType Test 7: MIME type");

    // Test 8: No extension
    result = verifyMimeType("/path/to/document", mimeType, sizeof(mimeType));
    ASSERT_EQ(1, result, "verifyMimeType Test 8: No extension");
    ASSERT_STR_EQ("application/octet-stream", mimeType, "verifyMimeType Test 8: No extension MIME type");

    printf("\nAll verifyMimeType tests completed.\n");

    return 0;
}
