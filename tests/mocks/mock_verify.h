#ifndef MOCK_VERIFY_H
#define MOCK_VERIFY_H

#include <stddef.h> // Para size_t

// Função para definir o valor de retorno para mock_verifyMimeType
void set_mock_verifyMimeType_return(const char *mime_type);

// Mock para a função verifyMimeType
void mock_verifyMimeType(const char *path, char *mimeType, size_t mimeType_size);

#endif // MOCK_VERIFY_H
