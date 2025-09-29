#include "response.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <limits.h> // Para PATH_MAX
#include <stdlib.h> // Para realpath
#include "../../config.h"
#include "../../utils/verify.h" // Para verifyMimeType

// #ifdef TEST_BUILD
// #include "../tests/mocks/mock_syscalls.h"
// #define open __wrap_open
// #endif

int httpResponse(const char *path, char *status_msg, size_t status_msg_size, char *mimeType, size_t mimeType_size)
{
    int fd = open(path, O_RDONLY);

    if (fd != -1)
    {
        strncpy(status_msg, "200 OK", status_msg_size - 1);
        status_msg[status_msg_size - 1] = '\0';
        verifyMimeType(path, mimeType, mimeType_size);
    } else {
        strncpy(status_msg, "404 Not Found", status_msg_size - 1);
        status_msg[status_msg_size - 1] = '\0';
        strncpy(mimeType, "text/html", mimeType_size - 1);
        mimeType[mimeType_size - 1] = '\0';
    }

    return fd;
}
