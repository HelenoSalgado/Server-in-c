#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include "cache.h"

// Códigos de cor ANSI
#define VERDE "\033[32m"
#define VERMELHO "\033[31m"
#define AMARELO "\033[33m"
#define AZUL "\033[34m"
#define RESET "\033[0m"

void test_cache_basic_operations() {
    printf(AZUL "🧪 Testando operações básicas do cache...\n" RESET);
    
    cache_t cache;
    cache_init(&cache);
    
    // Teste 1: Cache vazio
    cache_entry_t *entry = cache_get(&cache, "/test.html");
    assert(entry == NULL);
    printf(VERDE "✓ PASSOU: Cache vazio retorna NULL\n" RESET);
    
    // Teste 2: Adicionar item ao cache
    const char *test_data = "<html><body>Test</body></html>";
    int result = cache_put(&cache, "/test.html", test_data, strlen(test_data), "text/html");
    assert(result == 0);
    printf(VERDE "✓ PASSOU: Item adicionado ao cache\n" RESET);
    
    // Teste 3: Recuperar item do cache
    entry = cache_get(&cache, "/test.html");
    assert(entry != NULL);
    assert(strcmp(entry->path, "/test.html") == 0);
    assert(strcmp(entry->mime_type, "text/html") == 0);
    assert(entry->size == strlen(test_data));
    assert(strcmp(entry->data, test_data) == 0);
    printf(VERDE "✓ PASSOU: Item recuperado do cache corretamente\n" RESET);
    
    // Teste 4: Estatísticas do cache
    int entries, total_size;
    cache_stats(&cache, &entries, &total_size);
    assert(entries == 1);
    assert(total_size == strlen(test_data));
    printf(VERDE "✓ PASSOU: Estatísticas do cache corretas (%d entradas, %d bytes)\n" RESET, entries, total_size);
    
    cache_clear(&cache);
    printf(VERDE "✓ Cache limpo com sucesso\n" RESET);
}

void test_cache_ttl() {
    printf(AZUL "\n🕒 Testando TTL (Time To Live) do cache...\n" RESET);
    
    cache_t cache;
    cache_init(&cache);
    
    const char *test_data = "Test data for TTL";
    cache_put(&cache, "/ttl-test.txt", test_data, strlen(test_data), "text/plain");
    
    // Verifica se está no cache
    cache_entry_t *entry = cache_get(&cache, "/ttl-test.txt");
    assert(entry != NULL);
    printf(VERDE "✓ PASSOU: Item adicionado e encontrado no cache\n" RESET);
    
    // Simula expiração modificando o timestamp manualmente
    entry->timestamp -= (CACHE_TTL_SECONDS + 1);
    
    // Agora deve retornar NULL (expirado)
    entry = cache_get(&cache, "/ttl-test.txt");
    assert(entry == NULL);
    printf(VERDE "✓ PASSOU: Item expirado removido automaticamente\n" RESET);
    
    cache_clear(&cache);
}

void test_cache_capacity() {
    printf(AZUL "\n📦 Testando capacidade e substituição do cache...\n" RESET);
    
    cache_t cache;
    cache_init(&cache);
    
    // Adiciona alguns itens
    for (int i = 0; i < 5; i++) {
        char path[50];
        char data[100];
        snprintf(path, sizeof(path), "/file%d.txt", i);
        snprintf(data, sizeof(data), "Content of file %d", i);
        
        int result = cache_put(&cache, path, data, strlen(data), "text/plain");
        assert(result == 0);
    }
    
    int entries, total_size;
    cache_stats(&cache, &entries, &total_size);
    assert(entries == 5);
    printf(VERDE "✓ PASSOU: %d itens adicionados ao cache\n" RESET, entries);
    
    // Verifica se todos os itens estão acessíveis
    for (int i = 0; i < 5; i++) {
        char path[50];
        snprintf(path, sizeof(path), "/file%d.txt", i);
        
        cache_entry_t *entry = cache_get(&cache, path);
        assert(entry != NULL);
    }
    printf(VERDE "✓ PASSOU: Todos os itens são acessíveis\n" RESET);
    
    cache_clear(&cache);
}

void test_cache_invalid_operations() {
    printf(AZUL "\n❌ Testando operações inválidas...\n" RESET);
    
    cache_t cache;
    cache_init(&cache);
    
    // Teste com parâmetros NULL
    int result = cache_put(NULL, "/test.txt", "data", 4, "text/plain");
    assert(result == -1);
    printf(VERDE "✓ PASSOU: cache_put rejeita cache NULL\n" RESET);
    
    result = cache_put(&cache, NULL, "data", 4, "text/plain");
    assert(result == -1);
    printf(VERDE "✓ PASSOU: cache_put rejeita path NULL\n" RESET);
    
    result = cache_put(&cache, "/test.txt", NULL, 4, "text/plain");
    assert(result == -1);
    printf(VERDE "✓ PASSOU: cache_put rejeita data NULL\n" RESET);
    
    // Teste com arquivo muito grande
    char large_data[CACHE_MAX_FILE_SIZE + 100];
    memset(large_data, 'A', sizeof(large_data) - 1);
    large_data[sizeof(large_data) - 1] = '\0';
    
    result = cache_put(&cache, "/large.txt", large_data, sizeof(large_data) - 1, "text/plain");
    assert(result == -1);
    printf(VERDE "✓ PASSOU: cache_put rejeita arquivo muito grande\n" RESET);
    
    cache_clear(&cache);
}

int main() {
    printf(AZUL "🚀 Iniciando testes do sistema de cache...\n\n" RESET);
    
    test_cache_basic_operations();
    test_cache_ttl();
    test_cache_capacity();
    test_cache_invalid_operations();
    
    printf(AMARELO "\n✅ Todos os testes de cache concluídos com sucesso!\n" RESET);
    return 0;
}