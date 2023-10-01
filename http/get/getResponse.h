#ifndef GET_RESPONSE
#define GET_RESPONSE

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "./../../config.h"

extern char headers[500],
            mimeType[MIMETYPE_SIZE],
            status_msg[STATUS_SIZE];


int httpResponse(char *path);

#endif