#include "./../config.h"
#include "./verifyMimeType.h"
#include "request.h"

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>

int request(char *method, char *path){

    char *fileName;
    int fd, sizeFile;

    // Verifica qual é o tipo do recurso e adiciona-o na variável global mimeType
    verifyMimeType(path);

    if (strcmp(method, "GET") == 0){

        if ( strcmp(path, "/") == 0){

            // Abre o arquivo no diretório raiz definido
            fd = open(DIR_ROOT, O_RDONLY);
            strncpy(mimeType, "text/html", sizeof(mimeType));
            strncpy(status_msg, "200 OK", sizeof(status_msg));

        }else{

            if ( strcmp(mimeType, "text/html") == 0){

                // Concatena path com o arquivo de index
                sprintf(fileName, "%s%s%s", ".", path, "/index.html");
                
            }else{

                sprintf(fileName, "%s%s", ".", path);

            }


            fd = open(fileName, O_RDONLY);

            strncpy(status_msg, "200 OK", sizeof(status_msg));

            if (fd == -1){

                fd = open("./web/pages/notFound.html", O_RDONLY);
                strncpy(status_msg, "404 Not Found", sizeof(status_msg));
                
            }
        }
    }

    // Ler o arquivo e grava na variável global buffer
    sizeFile = read(fd, &buffer, sizeof(buffer));

    printf("%s\n", fileName);
    printf("%i\n", sizeFile);
    printf("%s\n", buffer);

    // Concatena o header com valores correspondentes ao arquivo
    snprintf(headers, sizeof(headers), "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %i\r\nConnection: close\r\n\r\n", status_msg, mimeType, sizeFile);

    // Fecha o arquivo
    close(fd);

    return 1;
}