#include "cache.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void cache_init(cache_t *cache) {
    if (!cache) return;
    
    memset(cache, 0, sizeof(cache_t));
    cache->count = 0;
    cache->next_index = 0;
}

cache_entry_t* cache_get(cache_t *cache, const char *path) {
    if (!cache || !path) return NULL;
    
    time_t current_time = time(NULL);
    
    for (int i = 0; i < CACHE_MAX_ENTRIES; i++) {
        cache_entry_t *entry = &cache->entries[i];
        
        if (!entry->is_valid) continue;
        
        // Verifica se a entrada expirou
        if (current_time - entry->timestamp > CACHE_TTL_SECONDS) {
            // Entrada expirada, marcar como inválida
            entry->is_valid = 0;
            if (entry->data) {
                free(entry->data);
                entry->data = NULL;
            }
            cache->count--;
            continue;
        }
        
        // Verifica se o caminho corresponde
        if (strcmp(entry->path, path) == 0) {
            return entry;
        }
    }
    
    return NULL;  // Não encontrado ou expirado
}

int cache_put(cache_t *cache, const char *path, const char *data, size_t size, const char *mime_type) {
    if (!cache || !path || !data || size == 0 || size > CACHE_MAX_FILE_SIZE) {
        return -1;  // Parâmetros inválidos ou arquivo muito grande
    }
    
    // Verifica se já existe (atualiza se existir)
    cache_entry_t *existing = cache_get(cache, path);
    if (existing) {
        // Atualiza entrada existente
        if (existing->data) {
            free(existing->data);
        }
        existing->data = malloc(size);
        if (!existing->data) return -1;
        
        memcpy(existing->data, data, size);
        existing->size = size;
        existing->timestamp = time(NULL);
        strncpy(existing->mime_type, mime_type, sizeof(existing->mime_type) - 1);
        existing->mime_type[sizeof(existing->mime_type) - 1] = '\0';
        return 0;
    }
    
    // Procura por slot vazio ou usa round-robin
    cache_entry_t *entry = NULL;
    
    // Primeiro, tenta encontrar um slot vazio
    for (int i = 0; i < CACHE_MAX_ENTRIES; i++) {
        if (!cache->entries[i].is_valid) {
            entry = &cache->entries[i];
            break;
        }
    }
    
    // Se não encontrou slot vazio, usa round-robin
    if (!entry) {
        entry = &cache->entries[cache->next_index];
        cache->next_index = (cache->next_index + 1) % CACHE_MAX_ENTRIES;
        
        // Libera dados antigos se existirem
        if (entry->data) {
            free(entry->data);
            entry->data = NULL;
        } else {
            cache->count++;  // Só incrementa se era um slot vazio
        }
    } else {
        cache->count++;
    }
    
    // Aloca memória para os dados
    entry->data = malloc(size);
    if (!entry->data) {
        entry->is_valid = 0;
        cache->count--;
        return -1;
    }
    
    // Preenche a entrada
    strncpy(entry->path, path, sizeof(entry->path) - 1);
    entry->path[sizeof(entry->path) - 1] = '\0';
    memcpy(entry->data, data, size);
    entry->size = size;
    strncpy(entry->mime_type, mime_type, sizeof(entry->mime_type) - 1);
    entry->mime_type[sizeof(entry->mime_type) - 1] = '\0';
    entry->timestamp = time(NULL);
    entry->is_valid = 1;
    
    return 0;
}

void cache_cleanup(cache_t *cache) {
    if (!cache) return;
    
    time_t current_time = time(NULL);
    
    for (int i = 0; i < CACHE_MAX_ENTRIES; i++) {
        cache_entry_t *entry = &cache->entries[i];
        
        if (entry->is_valid && (current_time - entry->timestamp > CACHE_TTL_SECONDS)) {
            entry->is_valid = 0;
            if (entry->data) {
                free(entry->data);
                entry->data = NULL;
            }
            cache->count--;
        }
    }
}

void cache_clear(cache_t *cache) {
    if (!cache) return;
    
    for (int i = 0; i < CACHE_MAX_ENTRIES; i++) {
        cache_entry_t *entry = &cache->entries[i];
        if (entry->data) {
            free(entry->data);
            entry->data = NULL;
        }
    }
    
    memset(cache, 0, sizeof(cache_t));
    cache->count = 0;
    cache->next_index = 0;
}

void cache_stats(cache_t *cache, int *entries, int *total_size) {
    if (!cache || !entries || !total_size) return;
    
    *entries = 0;
    *total_size = 0;
    
    for (int i = 0; i < CACHE_MAX_ENTRIES; i++) {
        cache_entry_t *entry = &cache->entries[i];
        if (entry->is_valid) {
            (*entries)++;
            *total_size += entry->size;
        }
    }
}