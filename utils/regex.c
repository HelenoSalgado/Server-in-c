#include "regex.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int regexPath(char *bufferClient, char *path, size_t path_size, char *method, size_t method_size)
{
    regex_t preg;
    // Este padrão captura o método e o caminho solicitado de uma requisição HTTP
    char *pattern = "^([A-Z]+) ([^ ]+) HTTP/1\\.1\\r?\\n?";
    regmatch_t match[3];

    if (regcomp(&preg, pattern, REG_EXTENDED) != 0) {
        perror("regcomp");
        return -1; // Falha na compilação
    }

    int result = regexec(&preg, bufferClient, 3, match, 0);

    if (result == 0)
    {
        // Extrai o método
        int method_len = match[1].rm_eo - match[1].rm_so;
        if (method_len > 0 && (size_t)method_len < method_size)
        {
            strncpy(method, bufferClient + match[1].rm_so, method_len);
            method[method_len] = '\0';
        }

        // Extrai o caminho da correspondência
        int len = match[2].rm_eo - match[2].rm_so;
        if (len > 0 && (size_t)len < path_size)
        {
            strncpy(path, bufferClient + match[2].rm_so, len);
            path[len] = '\0';
        } else {
            // Comprimento de caminho inválido ou caminho não encontrado
            regfree(&preg);
            return -1; // Indica falha
        }

        // Usa index.html como padrão se a raiz for solicitada
        if (strcmp(path, "/") == 0) {
            strncpy(path, "/index.html", path_size - 1);
            path[path_size - 1] = '\0';
        }

    } else {
        // Não correspondeu ao padrão de requisição GET
        regfree(&preg);
        return -1; // Indica falha
    }

    regfree(&preg);

    return 0; // Indica sucesso
}
