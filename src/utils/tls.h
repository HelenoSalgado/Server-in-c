/*
 * TLS/HTTPS Support Module
 * 
 * ⚠️  IMPLEMENTAÇÃO INCOMPLETA - REQUER MAIS DESENVOLVIMENTO
 * 
 * Este módulo contém a base para suporte HTTPS/TLS usando OpenSSL.
 * Funcionalidades implementadas parcialmente:
 * - Inicialização de contexto SSL
 * - Geração de certificados autoassinados
 * - Wrapper básico para operações SSL
 * 
 * TODO para completar:
 * - Integração completa com o servidor principal
 * - Tratamento de erros SSL robusto
 * - Configuração de ciphers seguros
 * - Suporte a SNI (Server Name Indication)
 * - Validação de certificados
 * - Testes automatizados
 * 
 * Para habilitar: adicionar dependência OpenSSL no build system
 */

#ifndef TLS_H
#define TLS_H

#include <openssl/ssl.h>
#include <openssl/err.h>

// Estrutura para gerenciar contexto TLS
typedef struct {
    SSL_CTX *ctx;
    int enabled;
    char *cert_file;
    char *key_file;
} tls_config_t;

// Inicializa o subsistema TLS
int tls_init(tls_config_t *config, const char *cert_file, const char *key_file);

// Cria uma conexão TLS para um socket
SSL* tls_accept(tls_config_t *config, int client_fd);

// Lê dados de uma conexão TLS
int tls_read(SSL *ssl, void *buf, int len);

// Escreve dados para uma conexão TLS
int tls_write(SSL *ssl, const void *buf, int len);

// Fecha uma conexão TLS
void tls_close(SSL *ssl);

// Limpa o contexto TLS
void tls_cleanup(tls_config_t *config);

// Gera certificado autoassinado se necessário
int tls_generate_self_signed_cert(const char *cert_file, const char *key_file);

#endif