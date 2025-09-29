#ifndef GET_RESPONSE_H
#define GET_RESPONSE_H

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "./../../config.h"

// Trata uma requisição GET para um dado caminho e retorna o descritor do arquivo.
// Preenche status_msg e mimeType com base no resultado.
int httpResponse(const char *path, char *status_msg, size_t status_msg_size, char *mimeType, size_t mimeType_size);

#endif