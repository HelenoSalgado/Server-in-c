#include <stdio.h>
#include <string.h>
#include <stdlib.h> // For system()
#include <unistd.h> // For getpid()
#include "../utils/path.h"

// Códigos de cor ANSI
#define VERDE "\033[32m"
#define VERMELHO "\033[31m"
#define AMARELO "\033[33m"
#define AZUL "\033[34m"
#define RESET "\033[0m"

#define ASSERT_EQ(expected, actual, message) do { \
    if ((expected) != (actual)) { \
        printf(VERMELHO "FALHOU: %s - Esperado %d, obtido %d\n" RESET, message, (int)(expected), (int)(actual)); \
        return 1; \
    } \
    printf(VERDE "✓ PASSOU: %s\n" RESET, message); \
} while(0)

#define ASSERT_STR_EQ(expected, actual, message) do { \
    if (strcmp((expected), (actual)) != 0) { \
        printf(VERMELHO "FALHOU: %s - Esperado \"%s\", obtido \"%s\"\n" RESET, message, (expected), (actual)); \
        return 1; \
    } \
    printf(VERDE "✓ PASSOU: %s\n" RESET, message); \
} while(0)

int main() {
    char sanitized_path[1024];
    int result;
    char base_dir[256];

    printf(AZUL "\n-- Executando Teste de Caminho --\n" RESET);

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
    ASSERT_EQ(0, result, "Caminho válido");
    char expected_path[1024];
    snprintf(expected_path, sizeof(expected_path), "%s/index.html", base_dir);
    ASSERT_STR_EQ(expected_path, sanitized_path, "Conteúdo do caminho sanitizado");

    // Test 2: Path traversal attempt (../)
    result = sanitize_path(base_dir, "/../etc/passwd", sanitized_path, sizeof(sanitized_path));
    ASSERT_EQ(-1, result, "Tentativa de travessia de caminho (../)");

    // Test 3: Path traversal attempt (absolute path)
    result = sanitize_path(base_dir, "/etc/passwd", sanitized_path, sizeof(sanitized_path));
    ASSERT_EQ(-1, result, "Tentativa de travessia de caminho (caminho absoluto)");

    // Test 4: Empty requested path
    result = sanitize_path(base_dir, "", sanitized_path, sizeof(sanitized_path));
    ASSERT_EQ(0, result, "Caminho requisitado vazio");
    snprintf(expected_path, sizeof(expected_path), "%s/", base_dir);
    ASSERT_STR_EQ(expected_path, sanitized_path, "Conteúdo do caminho sanitizado para requisição vazia");

    // Test 5: Buffer too small
    result = sanitize_path(base_dir, "/long/path/to/file.html", sanitized_path, 10); // Too small buffer
    ASSERT_EQ(-1, result, "Buffer muito pequeno");

    printf(AMARELO "\nTodos os testes de caminho concluídos.\n" RESET);

    // Clean up the temporary directory
    snprintf(command, sizeof(command), "rm -rf %s", base_dir);
    system(command);

    return 0;
}
