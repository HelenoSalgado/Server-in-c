#ifndef REGEX_H
#define REGEX_H

#include <regex.h>
#include <string.h>

// Takes the client buffer and extracts the path and method into the provided buffers.
int regexPath(char *bufferClient, char *path, size_t path_size, char *method, size_t method_size);

#endif