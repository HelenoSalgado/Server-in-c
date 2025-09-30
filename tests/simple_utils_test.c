#include <stdio.h>
#include <string.h>
#include "../utils/regex.h"
#include "../utils/verify.h"

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
} while(0)

#define ASSERT_STR_EQ(expected, actual, message) do { \
    if (strcmp((expected), (actual)) != 0) { \
        printf(VERMELHO "FALHOU: %s - Esperado \"%s\", obtido \"%s\"\n" RESET, message, (expected), (actual)); \
        return 1; \
    } \
} while(0)

#define TEST_REGEX_PATH(request, exp_result, exp_path, exp_method, test_name) do { \
    result = regexPath(request, path, sizeof(path), method, sizeof(method)); \
    ASSERT_EQ(exp_result, result, test_name); \
    if (exp_result == 0) { \
        ASSERT_STR_EQ(exp_path, path, test_name " - Caminho"); \
        ASSERT_STR_EQ(exp_method, method, test_name " - Método"); \
    } \
    printf(VERDE "✓ PASSOU: %s\n" RESET, test_name); \
} while(0)

#define TEST_MIME_TYPE(filepath, exp_result, exp_mimetype, test_name) do { \
    result = verifyMimeType(filepath, mimeType, sizeof(mimeType)); \
    ASSERT_EQ(exp_result, result, test_name); \
    ASSERT_STR_EQ(exp_mimetype, mimeType, test_name " - Tipo MIME"); \
    printf(VERDE "✓ PASSOU: %s\n" RESET, test_name); \
} while(0)

int main() {
    char path[256];
    char method[16];
    char mimeType[64];
    int result;

    printf(AZUL "\n-- Executando Testes de Utilitários --\n" RESET);

    // --- Testes para regexPath ---
    char request1[] = "GET /index.html HTTP/1.1\r\n";
    TEST_REGEX_PATH(request1, 0, "/index.html", "GET", "Requisição GET válida");

    char request2[] = "HEAD /data.json HTTP/1.1\r\n";
    TEST_REGEX_PATH(request2, 0, "/data.json", "HEAD", "Requisição HEAD válida");

    char request3[] = "POST /submit HTTP/1.1\r\n";
    TEST_REGEX_PATH(request3, 0, "/submit", "POST", "Método não suportado (POST)");

    char request4[] = "INVALID_REQUEST\r\n";
    result = regexPath(request4, path, sizeof(path), method, sizeof(method));
    ASSERT_EQ(-1, result, "Formato de requisição inválido");
    printf(VERDE "✓ PASSOU: Formato de requisição inválido\n" RESET);

    printf(AMARELO "Todos os testes regexPath concluídos.\n" RESET);

    // --- Testes para verifyMimeType ---
    TEST_MIME_TYPE("/caminho/para/index.html", 0, "text/html", "Arquivo HTML");
    TEST_MIME_TYPE("/caminho/para/style.css", 0, "text/css", "Arquivo CSS");
    TEST_MIME_TYPE("/caminho/para/script.js", 0, "application/javascript", "Arquivo JavaScript");
    TEST_MIME_TYPE("/caminho/para/icone.svg", 0, "image/svg+xml", "Arquivo SVG");
    TEST_MIME_TYPE("/caminho/para/config.xml", 0, "application/xml", "Arquivo XML");
    TEST_MIME_TYPE("/caminho/para/foto.webp", 0, "image/webp", "Arquivo WebP");
    TEST_MIME_TYPE("/caminho/para/documento.xyz", 0, "application/octet-stream", "Extensão desconhecida");
    TEST_MIME_TYPE("/caminho/para/documento", 1, "application/octet-stream", "Sem extensão");

    printf(AMARELO "Todos os testes verifyMimeType concluídos.\n" RESET);

    return 0;
}
