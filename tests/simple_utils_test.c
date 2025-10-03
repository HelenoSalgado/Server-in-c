#include <stdio.h>
#include <string.h>
#include "regex.h"
#include "verify.h"

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
    // Tipos básicos já existentes
    TEST_MIME_TYPE("/caminho/para/index.html", 0, "text/html", "Arquivo HTML");
    TEST_MIME_TYPE("/caminho/para/style.css", 0, "text/css", "Arquivo CSS");
    TEST_MIME_TYPE("/caminho/para/script.js", 0, "application/javascript", "Arquivo JavaScript");
    TEST_MIME_TYPE("/caminho/para/icone.svg", 0, "image/svg+xml", "Arquivo SVG");
    TEST_MIME_TYPE("/caminho/para/config.xml", 0, "application/xml", "Arquivo XML");
    TEST_MIME_TYPE("/caminho/para/foto.webp", 0, "image/webp", "Arquivo WebP");
    TEST_MIME_TYPE("/caminho/para/data.json", 0, "application/json", "Arquivo JSON");
    
    // Novos tipos de imagem
    TEST_MIME_TYPE("/caminho/para/foto.png", 0, "image/png", "Arquivo PNG");
    TEST_MIME_TYPE("/caminho/para/foto.jpg", 0, "image/jpeg", "Arquivo JPEG");
    TEST_MIME_TYPE("/caminho/para/foto.jpeg", 0, "image/jpeg", "Arquivo JPEG (extensão completa)");
    TEST_MIME_TYPE("/caminho/para/animacao.gif", 0, "image/gif", "Arquivo GIF");
    TEST_MIME_TYPE("/caminho/para/favicon.ico", 0, "image/x-icon", "Arquivo ICO");
    TEST_MIME_TYPE("/caminho/para/imagem.bmp", 0, "image/bmp", "Arquivo BMP");
    
    // Novos tipos de fonte
    TEST_MIME_TYPE("/caminho/para/fonte.woff", 0, "font/woff", "Arquivo WOFF");
    TEST_MIME_TYPE("/caminho/para/fonte.woff2", 0, "font/woff2", "Arquivo WOFF2");
    TEST_MIME_TYPE("/caminho/para/fonte.ttf", 0, "font/ttf", "Arquivo TTF");
    TEST_MIME_TYPE("/caminho/para/fonte.otf", 0, "font/otf", "Arquivo OTF");
    TEST_MIME_TYPE("/caminho/para/fonte.eot", 0, "application/vnd.ms-fontobject", "Arquivo EOT");
    
    // Novos tipos de mídia
    TEST_MIME_TYPE("/caminho/para/video.mp4", 0, "video/mp4", "Arquivo MP4");
    TEST_MIME_TYPE("/caminho/para/video.webm", 0, "video/webm", "Arquivo WebM");
    TEST_MIME_TYPE("/caminho/para/audio.mp3", 0, "audio/mpeg", "Arquivo MP3");
    TEST_MIME_TYPE("/caminho/para/audio.wav", 0, "audio/wav", "Arquivo WAV");
    TEST_MIME_TYPE("/caminho/para/audio.ogg", 0, "audio/ogg", "Arquivo OGG");
    
    // Novos tipos de documento
    TEST_MIME_TYPE("/caminho/para/documento.pdf", 0, "application/pdf", "Arquivo PDF");
    TEST_MIME_TYPE("/caminho/para/texto.txt", 0, "text/plain", "Arquivo TXT");
    TEST_MIME_TYPE("/caminho/para/readme.md", 0, "text/markdown", "Arquivo Markdown");
    
    // Novos tipos de arquivo compactado
    TEST_MIME_TYPE("/caminho/para/arquivo.zip", 0, "application/zip", "Arquivo ZIP");
    TEST_MIME_TYPE("/caminho/para/arquivo.gz", 0, "application/gzip", "Arquivo GZ");
    
    // Casos especiais
    TEST_MIME_TYPE("/caminho/para/documento.xyz", 0, "application/octet-stream", "Extensão desconhecida");
    TEST_MIME_TYPE("/caminho/para/documento", 1, "application/octet-stream", "Sem extensão");

    printf(AMARELO "Todos os testes verifyMimeType concluídos.\n" RESET);

    return 0;
}
