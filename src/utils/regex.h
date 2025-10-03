#ifndef REGEX_H
#define REGEX_H

#include <string.h>

// Extrai o caminho e o método do buffer do cliente para os buffers fornecidos.
// Implementação simplificada sem dependência de regex POSIX
int regexPath(char *bufferClient, char *path, size_t path_size, char *method, size_t method_size);

#endif