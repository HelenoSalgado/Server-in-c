#ifndef MOCK_VERIFY_H
#define MOCK_VERIFY_H

#include <stddef.h> // For size_t

// Function to set the return value for mock_verifyMimeType
void set_mock_verifyMimeType_return(const char *mime_type);

// Mock function for verifyMimeType
void mock_verifyMimeType(const char *path, char *mimeType, size_t mimeType_size);

#endif // MOCK_VERIFY_H
