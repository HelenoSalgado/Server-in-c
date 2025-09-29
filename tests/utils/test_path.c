#include "../unity/unity.h"
#include "../../utils/path.h"
#include <string.h> 
#include <limits.h>

// Mock para DIR_ROOT, que é uma variável global em config.h
const char *DIR_ROOT = "/tmp/webroot";

void setUp(void) {
    // Criar um diretório e um arquivo de teste
    system("mkdir -p /tmp/webroot/css");
    system("touch /tmp/webroot/index.html");
    system("touch /tmp/webroot/css/style.css");
}

void tearDown(void) {
    // Limpar o diretório de teste
    system("rm -rf /tmp/webroot");
}

void test_sanitize_path_valid_path(void) {
    char sanitized_path[PATH_MAX];
    int result = sanitize_path(DIR_ROOT, "/index.html", sanitized_path, sizeof(sanitized_path));
    
    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_STRING("/tmp/webroot/index.html", sanitized_path);
}

void test_sanitize_path_root_path(void) {
    char sanitized_path[PATH_MAX];
    int result = sanitize_path(DIR_ROOT, "/", sanitized_path, sizeof(sanitized_path));

    TEST_ASSERT_EQUAL_INT(0, result);
    TEST_ASSERT_EQUAL_STRING("/tmp/webroot/index.html", sanitized_path);
}

void test_sanitize_path_traversal_attack(void) {
    char sanitized_path[PATH_MAX];
    int result = sanitize_path(DIR_ROOT, "/../../../../etc/passwd", sanitized_path, sizeof(sanitized_path));

    TEST_ASSERT_EQUAL_INT(-1, result);
}

void test_sanitize_path_non_existent_file(void) {
    char sanitized_path[PATH_MAX];
    // O caminho é seguro, mas o arquivo não existe. realpath falhará.
    int result = sanitize_path(DIR_ROOT, "/nonexistent.js", sanitized_path, sizeof(sanitized_path));

    TEST_ASSERT_EQUAL_INT(-1, result); 
}

void test_sanitize_path_absolute_path_attack(void) {
    char sanitized_path[PATH_MAX];
    int result = sanitize_path(DIR_ROOT, "/etc/passwd", sanitized_path, sizeof(sanitized_path));

    TEST_ASSERT_EQUAL_INT(-1, result);
}

int main(void) {
    UNITY_BEGIN();
    printf("\x1b[36m\x1b[3m\nTesting: sanitize_path function\n\x1b[0m");

    printf("\x1b[36m\x1b[3m  - Expects to correctly sanitize a valid path\n\x1b[0m");
    RUN_TEST(test_sanitize_path_valid_path);

    printf("\x1b[36m\x1b[3m  - Expects to handle the root path by serving index.html\n\x1b[0m");
    RUN_TEST(test_sanitize_path_root_path);

    printf("\x1b[36m\x1b[3m  - Expects to block a path traversal attack\n\x1b[0m");
    RUN_TEST(test_sanitize_path_traversal_attack);

    printf("\x1b[36m\x1b[3m  - Expects to fail for a non-existent file\n\x1b[0m");
    RUN_TEST(test_sanitize_path_non_existent_file);

    printf("\x1b[36m\x1b[3m  - Expects to block an absolute path attack\n\x1b[0m");
    RUN_TEST(test_sanitize_path_absolute_path_attack);

    return UNITY_END();
}
