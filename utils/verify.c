#include "../config.h"
#include "verify.h"
#include <stdio.h>
#include <string.h>

int verifyMimeType(const char *path, char *mimeType, size_t mimeType_size)
{
    // Encontra o último ponto no caminho para obter a extensão
    const char *dot = strrchr(path, '.');

    // Se não houver ponto, ou se for o primeiro caractere, não pode determinar o tipo.
    if (!dot || dot == path)
    {
        strncpy(mimeType, "application/octet-stream", mimeType_size - 1);
        mimeType[mimeType_size - 1] = '\0';
        return 1;
    }

    // A extensão começa logo após o ponto
    const char *extension = dot + 1;

    // Verifica especificamente por .tar.gz
    if (strlen(path) >= 7 && strcmp(path + strlen(path) - 7, ".tar.gz") == 0)
    {
        strncpy(mimeType, "application/gzip", mimeType_size - 1);
    }
    else if (strcmp(extension, "html") == 0)
    {
        strncpy(mimeType, "text/html", mimeType_size - 1);
    } else if (strcmp(extension, "css") == 0)
    {
        strncpy(mimeType, "text/css", mimeType_size - 1);
    } else if (strcmp(extension, "js") == 0)
    {
        strncpy(mimeType, "application/javascript", mimeType_size - 1);
    } else if (strcmp(extension, "svg") == 0)
    {
        strncpy(mimeType, "image/svg+xml", mimeType_size - 1);
    } else if (strcmp(extension, "webp") == 0)
    {
        strncpy(mimeType, "image/webp", mimeType_size - 1);
    } else if (strcmp(extension, "json") == 0)
    {
        strncpy(mimeType, "application/json", mimeType_size - 1);
    } else {
        // Padrão para extensões desconhecidas
        strncpy(mimeType, "application/octet-stream", mimeType_size - 1);
    }
    mimeType[mimeType_size - 1] = '\0';

    return 0;
}
