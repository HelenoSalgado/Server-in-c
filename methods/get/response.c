#include "response.h"
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "../../config.h"

int httpResponse(const char *path, char *status_msg, size_t status_msg_size, char *mimeType, size_t mimeType_size)
{
    char fileName[255];
    int fd;

    if (path[strlen(path) - 1] == '/')
    {
        snprintf(fileName, sizeof(fileName), "%s%s%s", DIR_ROOT, path, FILE_INDEX);
    } else {
        snprintf(fileName, sizeof(fileName), "%s%s", DIR_ROOT, path);
    }

    fd = open(fileName, O_RDONLY);

    if (fd != -1)
    {
        strncpy(status_msg, "200 OK", status_msg_size - 1);
        status_msg[status_msg_size - 1] = '\0';
    } else {
        perror("Failed to open requested file");
        fd = open(PAGE_NOT_FOUND, O_RDONLY);
        strncpy(status_msg, "404 Not Found", status_msg_size - 1);
        status_msg[status_msg_size - 1] = '\0';

        if (fd != -1)
        {
            strncpy(mimeType, "text/html", mimeType_size - 1);
            mimeType[mimeType_size - 1] = '\0';
        }
    }

    return fd;
}
