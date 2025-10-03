#include "config.h"
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
    } else if (strcmp(extension, "xml") == 0)
    {
        strncpy(mimeType, "application/xml", mimeType_size - 1);
    } else if (strcmp(extension, "webp") == 0)
    {
        strncpy(mimeType, "image/webp", mimeType_size - 1);
    } else if (strcmp(extension, "json") == 0)
    {
        strncpy(mimeType, "application/json", mimeType_size - 1);
    }
    // === Suporte a Imagens ===
    else if (strcmp(extension, "png") == 0)
    {
        strncpy(mimeType, "image/png", mimeType_size - 1);
    } else if (strcmp(extension, "jpg") == 0 || strcmp(extension, "jpeg") == 0)
    {
        strncpy(mimeType, "image/jpeg", mimeType_size - 1);
    } else if (strcmp(extension, "gif") == 0)
    {
        strncpy(mimeType, "image/gif", mimeType_size - 1);
    } else if (strcmp(extension, "ico") == 0)
    {
        strncpy(mimeType, "image/x-icon", mimeType_size - 1);
    } else if (strcmp(extension, "bmp") == 0)
    {
        strncpy(mimeType, "image/bmp", mimeType_size - 1);
    }
    // === Suporte a Fontes ===
    else if (strcmp(extension, "woff") == 0)
    {
        strncpy(mimeType, "font/woff", mimeType_size - 1);
    } else if (strcmp(extension, "woff2") == 0)
    {
        strncpy(mimeType, "font/woff2", mimeType_size - 1);
    } else if (strcmp(extension, "ttf") == 0)
    {
        strncpy(mimeType, "font/ttf", mimeType_size - 1);
    } else if (strcmp(extension, "otf") == 0)
    {
        strncpy(mimeType, "font/otf", mimeType_size - 1);
    } else if (strcmp(extension, "eot") == 0)
    {
        strncpy(mimeType, "application/vnd.ms-fontobject", mimeType_size - 1);
    }
    // === Suporte a Mídia ===
    else if (strcmp(extension, "mp4") == 0)
    {
        strncpy(mimeType, "video/mp4", mimeType_size - 1);
    } else if (strcmp(extension, "webm") == 0)
    {
        strncpy(mimeType, "video/webm", mimeType_size - 1);
    } else if (strcmp(extension, "mp3") == 0)
    {
        strncpy(mimeType, "audio/mpeg", mimeType_size - 1);
    } else if (strcmp(extension, "wav") == 0)
    {
        strncpy(mimeType, "audio/wav", mimeType_size - 1);
    } else if (strcmp(extension, "ogg") == 0)
    {
        strncpy(mimeType, "audio/ogg", mimeType_size - 1);
    }
    // === Suporte a Documentos ===
    else if (strcmp(extension, "pdf") == 0)
    {
        strncpy(mimeType, "application/pdf", mimeType_size - 1);
    } else if (strcmp(extension, "txt") == 0)
    {
        strncpy(mimeType, "text/plain", mimeType_size - 1);
    } else if (strcmp(extension, "md") == 0)
    {
        strncpy(mimeType, "text/markdown", mimeType_size - 1);
    }
    // === Suporte a Arquivos Compactados ===
    else if (strcmp(extension, "zip") == 0)
    {
        strncpy(mimeType, "application/zip", mimeType_size - 1);
    } else if (strcmp(extension, "gz") == 0)
    {
        strncpy(mimeType, "application/gzip", mimeType_size - 1);
    } else {
        // Padrão para extensões desconhecidas
        strncpy(mimeType, "application/octet-stream", mimeType_size - 1);
    }
    mimeType[mimeType_size - 1] = '\0';

    return 0;
}
