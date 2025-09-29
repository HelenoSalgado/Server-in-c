#include "../mocks/mock_verify.h"
#include "../unity/unity.h"
#include <string.h>

void setUp(void) {
    // Configurações antes de cada teste
}

void tearDown(void) {
    // Limpeza após cada teste
}

void test_verifyMimeType_html(void) {
    char mimeType[128];
    set_mock_verifyMimeType_return("text/html");
    verifyMimeType("/index.html", mimeType, sizeof(mimeType));
    TEST_ASSERT_EQUAL_STRING("text/html", mimeType);
}

void test_verifyMimeType_css(void) {
    char mimeType[128];
    set_mock_verifyMimeType_return("text/css");
    verifyMimeType("/style.css", mimeType, sizeof(mimeType));
    TEST_ASSERT_EQUAL_STRING("text/css", mimeType);
}

void test_verifyMimeType_js(void) {
    char mimeType[128];
    set_mock_verifyMimeType_return("text/javascript");
    verifyMimeType("/script.js", mimeType, sizeof(mimeType));
    TEST_ASSERT_EQUAL_STRING("text/javascript", mimeType);
}

void test_verifyMimeType_svg(void) {
    char mimeType[128];
    set_mock_verifyMimeType_return("image/svg+xml");
    verifyMimeType("/image.svg", mimeType, sizeof(mimeType));
    TEST_ASSERT_EQUAL_STRING("image/svg+xml", mimeType);
}

void test_verifyMimeType_webp(void) {
    char mimeType[128];
    set_mock_verifyMimeType_return("image/webp");
    verifyMimeType("/image.webp", mimeType, sizeof(mimeType));
    TEST_ASSERT_EQUAL_STRING("image/webp", mimeType);
}

void test_verifyMimeType_unknown(void) {
    char mimeType[128];
    set_mock_verifyMimeType_return("application/octet-stream");
    verifyMimeType("/file.unknown", mimeType, sizeof(mimeType));
    TEST_ASSERT_EQUAL_STRING("application/octet-stream", mimeType);
}

void test_verifyMimeType_no_extension(void) {
    char mimeType[128];
    set_mock_verifyMimeType_return("application/octet-stream");
    verifyMimeType("/file", mimeType, sizeof(mimeType));
    TEST_ASSERT_EQUAL_STRING("application/octet-stream", mimeType);
}

void test_verifyMimeType_tar_gz(void) {
    char mimeType[128];
    set_mock_verifyMimeType_return("application/gzip");
    verifyMimeType("/archive.tar.gz", mimeType, sizeof(mimeType));
    TEST_ASSERT_EQUAL_STRING("application/gzip", mimeType);
}

int main(void) {
    UNITY_BEGIN();
    printf("\x1b[36m\x1b[3m\nTesting: verifyMimeType function\n\x1b[0m");
    printf("\x1b[36m\x1b[3m  - Expects 'text/html' for .html extension\n\x1b[0m");
    RUN_TEST(test_verifyMimeType_html);
    printf("\x1b[36m\x1b[3m  - Expects 'text/css' for .css extension\n\x1b[0m");
    RUN_TEST(test_verifyMimeType_css);
    printf("\x1b[36m\x1b[3m  - Expects 'text/javascript' for .js extension\n\x1b[0m");
    RUN_TEST(test_verifyMimeType_js);
    printf("\x1b[36m\x1b[3m  - Expects 'image/svg+xml' for .svg extension\n\x1b[0m");
    RUN_TEST(test_verifyMimeType_svg);
    printf("\x1b[36m\x1b[3m  - Expects 'image/webp' for .webp extension\n\x1b[0m");
    RUN_TEST(test_verifyMimeType_webp);
    printf("\x1b[36m\x1b[3m  - Expects 'application/octet-stream' for unknown extension\n\x1b[0m");
    RUN_TEST(test_verifyMimeType_unknown);
    printf("\x1b[36m\x1b[3m  - Expects 'application/octet-stream' for no extension\n\x1b[0m");
    RUN_TEST(test_verifyMimeType_no_extension);
    printf("\x1b[36m\x1b[3m  - Expects 'application/gzip' for .tar.gz extension\n\x1b[0m");
    RUN_TEST(test_verifyMimeType_tar_gz);
    return UNITY_END();
}
