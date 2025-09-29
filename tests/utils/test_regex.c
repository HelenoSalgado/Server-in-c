#include "../../utils/regex.h"
#include "../unity/unity.h"
#include <string.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_regexPath_simpleGet(void) {
    char bufferClient[] = "GET /index.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    char path[256];
    char method[16];
    int result = regexPath(bufferClient, path, sizeof(path), method, sizeof(method));

    TEST_ASSERT_EQUAL_INT(0, result); // Assuming 0 indicates success
    TEST_ASSERT_EQUAL_STRING("/index.html", path);
    TEST_ASSERT_EQUAL_STRING("GET", method);
}

int main(void) {
    UNITY_BEGIN();
    printf("\x1b[36m\x1b[3m\nTesting: regexPath function\n\x1b[0m");
    printf("\x1b[36m\x1b[3m  - Expects to parse GET method and /index.html path from a simple HTTP request\n\x1b[0m");
    RUN_TEST(test_regexPath_simpleGet);
    return UNITY_END();
}
