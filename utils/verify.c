#include "../config.h"
#include "verify.h"
#include <stdio.h>
#include <string.h>

int verifyMimeType(const char *path, char *mimeType, size_t mimeType_size)
{
    // Find the last dot in the path to get the extension
    const char *dot = strrchr(path, '.');

    // If there's no dot, or it's the first character, we can't determine the type.
    if (!dot || dot == path)
    {
        strncpy(mimeType, "application/octet-stream", mimeType_size - 1);
        mimeType[mimeType_size - 1] = '\0';
        return 1;
    }

    // The extension starts right after the dot
    const char *extension = dot + 1;

    if (strcmp(extension, "html") == 0)
    {
        strncpy(mimeType, "text/html", mimeType_size - 1);
    } else if (strcmp(extension, "css") == 0)
    {
        strncpy(mimeType, "text/css", mimeType_size - 1);
    } else if (strcmp(extension, "js") == 0)
    {
        strncpy(mimeType, "text/javascript", mimeType_size - 1);
    } else if (strcmp(extension, "svg") == 0)
    {
        strncpy(mimeType, "image/svg+xml", mimeType_size - 1);
    } else if (strcmp(extension, "webp") == 0)
    {
        strncpy(mimeType, "image/webp", mimeType_size - 1);
    } else {
        // Default for unknown extensions
        strncpy(mimeType, "application/octet-stream", mimeType_size - 1);
    }
    mimeType[mimeType_size - 1] = '\0';

    return 0;
}