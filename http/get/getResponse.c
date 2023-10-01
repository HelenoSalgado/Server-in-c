#include "getResponse.h"

int httpResponse(char *path){

    char fileName[255];
    int fd;

    // Concatena o diretório raíz + path + recurso solicitado
    sprintf(fileName, "%s%s%s", DIR_ROOT, path, FILE_INDEX);

    if ( strcmp(mimeType, "text/html") == 0){

        fd = open(fileName, O_RDONLY);
        
        strncpy(status_msg, "200 OK", sizeof(status_msg));
                
    }else{

        sprintf(fileName, "%s%s", DIR_ROOT, path);

        fd = open(fileName, O_RDONLY);
        
        strncpy(status_msg, "200 OK", sizeof(status_msg));

    }


    if (fd == -1){

        printf("Valor do erro: %d\n", errno);
        perror("Erro");

        fd = open(PAGE_NOT_FOUND, O_RDONLY);
        strncpy(status_msg, "404 Not Found", sizeof(status_msg));
                
    }

    return fd;
}