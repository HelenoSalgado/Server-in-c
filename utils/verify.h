/*
* Determines the MIME type of a resource based on its path.
*/

#ifndef MIMETYPE_H // Changed from MIMITYPE to MIMETYPE for correctness
#define MIMETYPE_H

#include <string.h>

// Determines the MIME type from the path and writes it to the mimeType buffer.
int verifyMimeType(const char *path, char *mimeType, size_t mimeType_size);

#endif