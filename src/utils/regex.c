#include "regex.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

// Implementação simples sem dependência de regex POSIX
int regexPath(char *bufferClient, char *path, size_t path_size, char *method, size_t method_size)
{
    if (!bufferClient || !path || !method) {
        return -1;
    }

    // Procura pelo primeiro espaço (após o método)
    char *space1 = strchr(bufferClient, ' ');
    if (!space1) return -1;

    // Procura pelo segundo espaço (após o path)
    char *space2 = strchr(space1 + 1, ' ');
    if (!space2) return -1;

    // Verifica se é uma requisição HTTP válida
    if (strncmp(space2 + 1, "HTTP/", 5) != 0) return -1;

    // Extrair método
    size_t method_len = space1 - bufferClient;
    if (method_len >= method_size) return -1;
    
    strncpy(method, bufferClient, method_len);
    method[method_len] = '\0';

    // Extrair caminho
    size_t path_len = space2 - space1 - 1;
    if (path_len >= path_size) return -1;
    
    strncpy(path, space1 + 1, path_len);
    path[path_len] = '\0';

    return 0;
}
