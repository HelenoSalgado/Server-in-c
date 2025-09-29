#include "mock_verify.h"
#include <string.h>
#include <stdio.h>

static const char *mock_verifyMimeType_return_value = NULL;

void set_mock_verifyMimeType_return(const char *mime_type) {
    mock_verifyMimeType_return_value = mime_type;
}

void mock_verifyMimeType(const char *path, char *mimeType, size_t mimeType_size) {
    if (mock_verifyMimeType_return_value != NULL) {
        strncpy(mimeType, mock_verifyMimeType_return_value, mimeType_size - 1);
        mimeType[mimeType_size - 1] = '\0';
    } else {
        // Fallback to a default or indicate an error if not explicitly set
        strncpy(mimeType, "application/octet-stream", mimeType_size - 1);
        mimeType[mimeType_size - 1] = '\0';
    }
    // Optionally, you can add assertions here to check the 'path' argument
    // For example: TEST_ASSERT_EQUAL_STRING("/index.html", path);
}
