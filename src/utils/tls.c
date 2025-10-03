/*
 * TLS/HTTPS Support Implementation
 * 
 * ⚠️  IMPLEMENTAÇÃO INCOMPLETA - NÃO INCLUÍDA NO BUILD
 * 
 * Para compilar este módulo, adicione as dependências:
 * - OpenSSL development headers (libssl-dev)
 * - Link flags: -lssl -lcrypto
 */

#include "tls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

int tls_init(tls_config_t *config, const char *cert_file, const char *key_file) {
    if (!config) return -1;
    
    // Inicializa OpenSSL
    SSL_library_init();
    SSL_load_error_strings();
    OpenSSL_add_all_algorithms();
    
    // Cria contexto SSL
    config->ctx = SSL_CTX_new(TLS_server_method());
    if (!config->ctx) {
        fprintf(stderr, "Erro ao criar contexto SSL\n");
        return -1;
    }
    
    // Configura opções de segurança
    SSL_CTX_set_options(config->ctx, SSL_OP_SINGLE_DH_USE);
    
    // Se não foram fornecidos certificados, tenta gerar autoassinados
    if (!cert_file || !key_file) {
        cert_file = "server.crt";
        key_file = "server.key";
        
        // Verifica se os arquivos existem
        struct stat st;
        if (stat(cert_file, &st) != 0 || stat(key_file, &st) != 0) {
            printf("📋 Gerando certificado autoassinado...\n");
            if (tls_generate_self_signed_cert(cert_file, key_file) != 0) {
                fprintf(stderr, "Erro ao gerar certificado autoassinado\n");
                return -1;
            }
        }
    }
    
    // Carrega o certificado
    if (SSL_CTX_use_certificate_file(config->ctx, cert_file, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    
    // Carrega a chave privada
    if (SSL_CTX_use_PrivateKey_file(config->ctx, key_file, SSL_FILETYPE_PEM) <= 0) {
        ERR_print_errors_fp(stderr);
        return -1;
    }
    
    // Verifica se a chave privada corresponde ao certificado
    if (!SSL_CTX_check_private_key(config->ctx)) {
        fprintf(stderr, "Chave privada não corresponde ao certificado\n");
        return -1;
    }
    
    config->enabled = 1;
    config->cert_file = strdup(cert_file);
    config->key_file = strdup(key_file);
    
    printf("✅ TLS inicializado com certificado: %s\n", cert_file);
    return 0;
}

SSL* tls_accept(tls_config_t *config, int client_fd) {
    if (!config || !config->enabled) return NULL;
    
    SSL *ssl = SSL_new(config->ctx);
    if (!ssl) return NULL;
    
    SSL_set_fd(ssl, client_fd);
    
    if (SSL_accept(ssl) <= 0) {
        ERR_print_errors_fp(stderr);
        SSL_free(ssl);
        return NULL;
    }
    
    return ssl;
}

int tls_read(SSL *ssl, void *buf, int len) {
    if (!ssl) return -1;
    return SSL_read(ssl, buf, len);
}

int tls_write(SSL *ssl, const void *buf, int len) {
    if (!ssl) return -1;
    return SSL_write(ssl, buf, len);
}

void tls_close(SSL *ssl) {
    if (ssl) {
        SSL_shutdown(ssl);
        SSL_free(ssl);
    }
}

void tls_cleanup(tls_config_t *config) {
    if (!config) return;
    
    if (config->ctx) {
        SSL_CTX_free(config->ctx);
    }
    
    if (config->cert_file) {
        free(config->cert_file);
    }
    
    if (config->key_file) {
        free(config->key_file);
    }
    
    EVP_cleanup();
    ERR_free_strings();
    
    memset(config, 0, sizeof(tls_config_t));
}

int tls_generate_self_signed_cert(const char *cert_file, const char *key_file) {
    // Comando para gerar certificado autoassinado
    char cmd[512];
    snprintf(cmd, sizeof(cmd), 
        "openssl req -x509 -newkey rsa:2048 -keyout '%s' -out '%s' -days 365 -nodes "
        "-subj '/C=BR/ST=State/L=City/O=Organization/OU=OrgUnit/CN=localhost' 2>/dev/null",
        key_file, cert_file);
    
    int result = system(cmd);
    if (result != 0) {
        fprintf(stderr, "Erro ao executar openssl. Certifique-se de que está instalado.\n");
        return -1;
    }
    
    return 0;
}