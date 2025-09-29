#include "regex.h"
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

int regexPath(char *bufferClient, char *path, size_t path_size, char *method, size_t method_size)
{
    regex_t preg;
    // This pattern captures the method and the requested path from an HTTP request
    char *pattern = "^([A-Z]+) ([^ ]+)";
    regmatch_t match[3];

    if (regcomp(&preg, pattern, REG_EXTENDED) != 0) {
        perror("regcomp");
        return -1; // Compilation failed
    }

    int result = regexec(&preg, bufferClient, 3, match, 0);

    if (result == 0)
    {
        // Extract method
        int method_len = match[1].rm_eo - match[1].rm_so;
        if (method_len > 0 && (size_t)method_len < method_size)
        {
            strncpy(method, bufferClient + match[1].rm_so, method_len);
            method[method_len] = '\0';
        }

        // Extract the path from the match
        int len = match[2].rm_eo - match[2].rm_so;
        if (len > 0 && (size_t)len < path_size)
        {
            strncpy(path, bufferClient + match[2].rm_so, len);
            path[len] = '\0';
        } else {
            // Invalid path length or no path found
            regfree(&preg);
            return -1; // Indicate failure
        }

        // Default to index.html if root is requested
        if (strcmp(path, "/") == 0) {
            strncpy(path, "/index.html", path_size - 1);
            path[path_size - 1] = '\0';
        }

    } else {
        // Did not match GET request pattern
        regfree(&preg);
        return -1; // Indicate failure
    }

    regfree(&preg);

    return 0; // Indicate success
}
