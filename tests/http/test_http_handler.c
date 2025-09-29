#include "../../http/http_handler.h"
#include "../../http/context.h"
#include "../mocks/mock_syscalls.h"
#include "../mocks/mock_verify.h" // Inclui o mock para verifyMimeType
#include "../../config.h" // Para DIR_ROOT, FILE_INDEX, PAGE_NOT_FOUND
#include <string.h>
#include <stdlib.h>
#include <gtest/gtest.h>

// Não há mais mock local para httpResponse

class HttpHandlerTest : public ::testing::Test {
protected:
    void SetUp() override {
        // Cria o diretório base para os testes
        system("mkdir -p /tmp/webroot");
        // Cria arquivos de teste que serão usados nas requisições
        system("touch /tmp/webroot/index.html");
        system("touch /tmp/webroot/data");

        // Reseta o estado do mock antes de cada teste
        set_mock_open_return(-1); // Padrão para arquivo não encontrado
        set_mock_write_return(-1);
        clear_mock_write_buffer();
        set_mock_read_buffer(NULL, 0);
        set_mock_realpath_fail();
        set_mock_verifyMimeType_return(NULL);
    }

    void TearDown() override {
        // Limpa o diretório de teste
        system("rm -rf /tmp/webroot");

        // Limpa quaisquer recursos alocados
        clear_mock_write_buffer();
        set_mock_read_buffer(NULL, 0);
        set_mock_realpath_return(NULL);
        clear_mock_realpath_sequence();
        set_mock_verifyMimeType_return(NULL);
    }
};

TEST_F(HttpHandlerTest, ValidGetRequest) {
    http_context ctx;
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 1;

    static char request_buffer[] = "GET /index.html HTTP/1.1\r\n";
    set_mock_read_buffer(request_buffer, strlen(request_buffer));

    DIR_ROOT = (char*)"/tmp/webroot";

    const char *realpath_sequence[] = {
        "/tmp/webroot",
        "/tmp/webroot/index.html"
    };
    set_mock_realpath_sequence(realpath_sequence, 2);
    set_mock_open_return(100); // Simulate a valid file descriptor
    set_mock_verifyMimeType_return("text/html"); // Simulate mime type detection

    const char* file_content = "<html><body><h1>Hello GET</h1></body></html>";
    set_mock_file_content_buffer(file_content, strlen(file_content));

    http_handler(&ctx);

    // printf("\n--- ctx.data for GET request ---\n%s\n-----------------------------------\n", ctx.data);

    ASSERT_TRUE(strstr(ctx.data, "HTTP/1.1 200 OK") != NULL);
    ASSERT_TRUE(strstr(ctx.data, "Content-Type: text/html") != NULL);
    ASSERT_TRUE(strstr(ctx.data, "Content-Length: 35") != NULL); // Length of file_content
    ASSERT_TRUE(strstr(ctx.data, file_content) != NULL);
}

TEST_F(HttpHandlerTest, InvalidRequest) {
    http_context ctx;
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 1;

    char request_buffer[] = "INVALID_REQUEST\r\n\r\n";
    set_mock_read_buffer(request_buffer, strlen(request_buffer));

    http_handler(&ctx);

    ASSERT_TRUE(strstr(ctx.data, "HTTP/1.1 400 Bad Request") != NULL);
}

TEST_F(HttpHandlerTest, UnsupportedMethod) {
    http_context ctx;
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 1;

    char request_buffer[] = "POST /data HTTP/1.1\r\nHost: localhost\r\n\r\n";
    set_mock_read_buffer(request_buffer, strlen(request_buffer));

    // Precisa simular um caminho válido para passar pela sanitização
    set_mock_realpath_return("/tmp/webroot/data");

    http_handler(&ctx);

    ASSERT_TRUE(strstr(ctx.data, "HTTP/1.1 501 Not Implemented") != NULL);
}

TEST_F(HttpHandlerTest, FileNotFound) {
    http_context ctx;
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 1;

    char request_buffer[] = "GET /nonexistent.html HTTP/1.1\r\nHost: localhost\r\n\r\n";
    set_mock_read_buffer(request_buffer, strlen(request_buffer));

    // realpath falha para o arquivo, mas o caminho base é válido
    set_mock_realpath_return("/tmp/webroot/nonexistent.html");

    // open() falhará (retorno padrão de -1 do setUp)
    set_mock_open_return(-1);

    http_handler(&ctx);

    ASSERT_TRUE(strstr(ctx.data, "HTTP/1.1 404 Not Found") != NULL);
}

TEST_F(HttpHandlerTest, HeadRequest) {
    http_context ctx;
    memset(&ctx, 0, sizeof(http_context));
    ctx.client_fd = 1;

    static char request_buffer[] = "HEAD /index.html HTTP/1.1\r\n";
    set_mock_read_buffer(request_buffer, strlen(request_buffer));

    DIR_ROOT = (char*)"/tmp/webroot";

    const char *realpath_sequence[] = {
        "/tmp/webroot",
        "/tmp/webroot/index.html"
    };
    set_mock_realpath_sequence(realpath_sequence, 2);

    set_mock_open_return(100);
    set_mock_verifyMimeType_return("text/html");

    const char* file_content = "<html><body><h1>Hello</h1></body></html>";
    set_mock_file_content_buffer(file_content, strlen(file_content));

    http_handler(&ctx);

    // printf("\n--- ctx.data for HEAD request ---\n%s\n-------------------------------------\n", ctx.data);

    ASSERT_TRUE(strstr(ctx.data, "HTTP/1.1 200 OK") != NULL);
    ASSERT_TRUE(strstr(ctx.data, "Content-Type: text/html") != NULL);
    ASSERT_TRUE(strstr(ctx.data, "Content-Length: 35") != NULL); // Length of file_content

    // For HEAD request, the body should not be present
    char *body_start = strstr(ctx.data, "\r\n\r\n");
    ASSERT_TRUE(body_start != NULL);
    ASSERT_TRUE(strcmp("", body_start + 4) == 0);
}
