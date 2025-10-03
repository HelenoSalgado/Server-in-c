#ifndef CACHE_H
#define CACHE_H

#include <time.h>
#include <stddef.h>

#define CACHE_MAX_ENTRIES 100
#define CACHE_MAX_PATH_LEN 256
#define CACHE_MAX_FILE_SIZE (1024 * 1024)  // 1MB máximo por arquivo
#define CACHE_TTL_SECONDS 300  // 5 minutos de TTL (Time To Live)

typedef struct {
    char path[CACHE_MAX_PATH_LEN];
    char *data;
    size_t size;
    char mime_type[30];
    time_t timestamp;
    int is_valid;
} cache_entry_t;

typedef struct {
    cache_entry_t entries[CACHE_MAX_ENTRIES];
    int count;
    int next_index;  // Para algoritmo round-robin simples
} cache_t;

// Inicializa o sistema de cache
void cache_init(cache_t *cache);

// Busca um arquivo no cache
cache_entry_t* cache_get(cache_t *cache, const char *path);

// Adiciona um arquivo ao cache
int cache_put(cache_t *cache, const char *path, const char *data, size_t size, const char *mime_type);

// Remove entradas expiradas do cache
void cache_cleanup(cache_t *cache);

// Limpa todo o cache
void cache_clear(cache_t *cache);

// Estatísticas do cache
void cache_stats(cache_t *cache, int *entries, int *total_size);

#endif