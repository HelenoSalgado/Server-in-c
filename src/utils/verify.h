/*
* Determina o tipo MIME de um recurso com base em seu caminho.
*/

#ifndef MIMETYPE_H // Corrigido de MIMITYPE para MIMETYPE
#define MIMETYPE_H

#include <string.h>

// Determina o tipo MIME a partir do caminho e o escreve no buffer mimeType.
int verifyMimeType(const char *path, char *mimeType, size_t mimeType_size);

#endif