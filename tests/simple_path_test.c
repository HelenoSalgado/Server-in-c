#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For system()
#include <unistd.h> // For getpid()
#include "../utils/path.h"

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
    char sanitized_path[1024];
    int result;
    char base_dir[256];

    // Create a temporary directory for testing
    snprintf(base_dir, sizeof(base_dir), "/tmp/test_webroot_%d", getpid());
    char command[512];
    snprintf(command, sizeof(command), "mkdir -p %s", base_dir);
    system(command);

    // Create a dummy index.html file for testing
    snprintf(command, sizeof(command), "touch %s/index.html", base_dir);
    system(command);

    // Test 1: Valid path
    result = sanitize_path(base_dir, "/index.html", sanitized_path, sizeof(sanitized_path));
    ASSERT_EQ(0, result, "Test 1: Valid path");
    char expected_path[1024];
    snprintf(expected_path, sizeof(expected_path), "%s/index.html", base_dir);
    ASSERT_STR_EQ(expected_path, sanitized_path, "Test 1: Sanitized path content");

    // Test 2: Path traversal attempt (../)
    result = sanitize_path(base_dir, "/../etc/passwd", sanitized_path, sizeof(sanitized_path));
    ASSERT_EQ(-1, result, "Test 2: Path traversal attempt (../)");

    // Test 3: Path traversal attempt (absolute path)
    result = sanitize_path(base_dir, "/etc/passwd", sanitized_path, sizeof(sanitized_path));
    ASSERT_EQ(-1, result, "Test 3: Path traversal attempt (absolute path)");

    // Test 4: Empty requested path
    result = sanitize_path(base_dir, "", sanitized_path, sizeof(sanitized_path));
    ASSERT_EQ(0, result, "Test 4: Empty requested path");
    snprintf(expected_path, sizeof(expected_path), "%s/", base_dir);
    ASSERT_STR_EQ(expected_path, sanitized_path, "Test 4: Sanitized path content for empty request");

    // Test 5: Buffer too small
    result = sanitize_path(base_dir, "/long/path/to/file.html", sanitized_path, 10); // Too small buffer
    ASSERT_EQ(-1, result, "Test 5: Buffer too small");

    printf("\nAll simple path tests completed.\n");

    // Clean up the temporary directory
    snprintf(command, sizeof(command), "rm -rf %s", base_dir);
    system(command);

    return 0;
}
