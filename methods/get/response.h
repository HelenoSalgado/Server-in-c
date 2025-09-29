#ifndef GET_RESPONSE_H
#define GET_RESPONSE_H

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "./../../config.h"

// Handles a GET request for a given path and returns the file descriptor.
// It populates status_msg and mimeType based on the result.
int httpResponse(const char *path, char *status_msg, size_t status_msg_size, char *mimeType, size_t mimeType_size);

#endif