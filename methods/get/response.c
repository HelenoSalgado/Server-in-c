#include "response.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h> // For PATH_MAX
#include <stdlib.h> // For realpath
#include "../../config.h"
#include "../../utils/verify.h" // For verifyMimeType

int httpResponse(const char *path, char *status_msg, size_t status_msg_size, char *mimeType, size_t mimeType_size)
{
    char full_path[PATH_MAX];
    char resolved_path[PATH_MAX];
    char web_root[PATH_MAX];
    int fd = -1;

    // Constrói o caminho completo do arquivo, tratando o caso de ser a raiz "/"
    // O path já vem tratado para /index.html se a requisição for para /
    snprintf(full_path, sizeof(full_path), "%s%s", DIR_ROOT, path);

    fprintf(stderr, "DEBUG: httpResponse - full_path: %s\n", full_path);

    // Resolve o caminho real e verifica a existência do arquivo
    if (realpath(full_path, resolved_path) != NULL)
    {
        // Obtém o caminho real do DIR_ROOT
        if (realpath(DIR_ROOT, web_root) != NULL)
        {
            // Verifica se o resolved_path está dentro do DIR_ROOT (segurança contra Path Traversal)
            if (strncmp(web_root, resolved_path, strlen(web_root)) == 0)
            {
                fd = open(resolved_path, O_RDONLY);
                if (fd != -1)
                {
                    strncpy(status_msg, "200 OK", status_msg_size - 1);
                    status_msg[status_msg_size - 1] = '\0';
                }
                else
                {
                    perror("Failed to open resolved file");
                    // Fallback para 404 se o arquivo resolvido não puder ser aberto
                    snprintf(full_path, sizeof(full_path), "%s%s", DIR_ROOT, PAGE_NOT_FOUND);
                    fd = open(full_path, O_RDONLY);
                    strncpy(status_msg, "404 Not Found", status_msg_size - 1);
                    status_msg[status_msg_size - 1] = '\0';
                }
            }
            else
            {
                // Path Traversal Attempt or DIR_ROOT resolution failed
                fprintf(stderr, "Path traversal attempt blocked or DIR_ROOT resolution failed: %s\n", path);
                // Serve 404
                snprintf(full_path, sizeof(full_path), "%s%s", DIR_ROOT, PAGE_NOT_FOUND);
                fd = open(full_path, O_RDONLY);
                strncpy(status_msg, "404 Not Found", status_msg_size - 1);
                status_msg[status_msg_size - 1] = '\0';
            }
        }
        else
        {
            perror("Failed to resolve web root path");
            // Serve 404
            snprintf(full_path, sizeof(full_path), "%s%s", DIR_ROOT, PAGE_NOT_FOUND);
            fd = open(full_path, O_RDONLY);
            strncpy(status_msg, "404 Not Found", status_msg_size - 1);
            status_msg[status_msg_size - 1] = '\0';
        }
    }
    else
    {
        // File does not exist or path is invalid
        perror("File does not exist or path is invalid");
        // Serve 404
        snprintf(full_path, sizeof(full_path), "%s%s", DIR_ROOT, PAGE_NOT_FOUND);
        fd = open(full_path, O_RDONLY);
        strncpy(status_msg, "404 Not Found", status_msg_size - 1);
        status_msg[status_msg_size - 1] = '\0';
    }

    // Se o 404.html foi aberto com sucesso, define o mimeType
    if (fd != -1 && strcmp(status_msg, "404 Not Found") == 0)
    {
        strncpy(mimeType, "text/html", mimeType_size - 1);
        mimeType[mimeType_size - 1] = '\0';
    }
    // Caso contrário, tenta determinar o mimeType do arquivo original
    else if (fd != -1 && strcmp(status_msg, "200 OK") == 0)
    {
        verifyMimeType(path, mimeType, mimeType_size);
    }

    return fd;
}
