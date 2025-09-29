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
        char requested_path[255];
        int len = match[2].rm_eo - match[2].rm_so;
        if (len > 0 && len < (int)sizeof(requested_path))
        {
            strncpy(requested_path, bufferClient + match[2].rm_so, len);
            requested_path[len] = '\0';
        } else {
            // Invalid path length
            strncpy(path, "/404.html", path_size - 1);
            path[path_size - 1] = '\0';
            regfree(&preg);
            return 1;
        }

        // Default to index.html if root is requested
        if (strcmp(requested_path, "/") == 0) {
            strcpy(requested_path, "/index.html");
        }

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "web%s", requested_path);

        char resolved_path[PATH_MAX];
        // Check if the file exists and get its canonical path
        if (realpath(full_path, resolved_path) != NULL)
        {
            char web_root[PATH_MAX];
            if (realpath("web", web_root) != NULL)
            {
                // Security check: ensure the resolved path is within the web root
                if (strncmp(web_root, resolved_path, strlen(web_root)) == 0)
                {
                    strncpy(path, requested_path, path_size - 1);
                    path[path_size - 1] = '\0';
                }
                else
                {
                    // Path Traversal Attempt!
                    fprintf(stderr, "Path traversal attempt blocked: %s\n", requested_path);
                    strncpy(path, "/404.html", path_size - 1);
                    path[path_size - 1] = '\0';
                }
            }
        }
        else
        {
            // File does not exist
            strncpy(path, "/404.html", path_size - 1);
            path[path_size - 1] = '\0';
        }
    } else {
        // Did not match GET request pattern
        strncpy(path, "/404.html", path_size - 1);
        path[path_size - 1] = '\0';
    }

    regfree(&preg);

    return 0;
}
