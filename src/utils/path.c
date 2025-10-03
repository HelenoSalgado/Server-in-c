#include "path.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

// #ifdef TEST_BUILD
// #include "../tests/mocks/mock_syscalls.h"
// #endif

int sanitize_path(const char *base_dir, const char *requested_path, char *sanitized_path, size_t sanitized_path_size) {
    char full_path[PATH_MAX];
    char resolved_path[PATH_MAX];
    char resolved_base_dir[PATH_MAX];
    int ret_val = 0;
    int snprintf_result;

    // 1. Resolve o caminho base para um caminho absoluto e canônico
    if (realpath(base_dir, resolved_base_dir) == NULL) {
        ret_val = -1;
        return ret_val;
    }

    // 2. Lida com o caso de requisitar o diretório raiz, servindo o index.html
    if (strcmp(requested_path, "/") == 0) {
        requested_path = "/index.html";
    } else if (strcmp(requested_path, "") == 0) { // Handle empty requested_path
        snprintf_result = snprintf(full_path, sizeof(full_path), "%s/", resolved_base_dir);
        if (snprintf_result >= (int)sizeof(full_path) || snprintf_result < 0) {
            ret_val = -1;
            return ret_val;
        }

        if (strlcpy(sanitized_path, full_path, sanitized_path_size) >= sanitized_path_size) {
            ret_val = -1; // Buffer muito pequeno
            return ret_val;
        }
        return ret_val;
    }

    // 3. Constrói o caminho completo, evitando double slashes
    if (requested_path[0] == '/') {
        snprintf_result = snprintf(full_path, sizeof(full_path), "%s%s", resolved_base_dir, requested_path);
    } else {
        snprintf_result = snprintf(full_path, sizeof(full_path), "%s/%s", resolved_base_dir, requested_path);
    }

    if (snprintf_result >= (int)sizeof(full_path) || snprintf_result < 0) {
        ret_val = -1; // Erro ou truncamento do caminho
        return ret_val;
    }

    // 4. Resolve o caminho completo para obter o caminho absoluto e canônico
    if (realpath(full_path, resolved_path) == NULL) {
        ret_val = -1; // Caminho inválido ou arquivo não existe
        return ret_val;
    }

    // 5. Verifica se o caminho resolvido está dentro do diretório base
    if (strncmp(resolved_path, resolved_base_dir, strlen(resolved_base_dir)) != 0) {
        ret_val = -1; // Tentativa de path traversal
        return ret_val;
    }

    // 6. Copia o caminho sanitizado para o buffer de saída
    if (strlcpy(sanitized_path, resolved_path, sanitized_path_size) >= sanitized_path_size) {
        ret_val = -1; // Buffer muito pequeno
        return ret_val;
    }

    return ret_val;
}
