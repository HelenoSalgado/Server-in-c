#include "./../config.h"
#include "./verifyMimeType.h"
#include "request.h"
#include "./get/getResponse.h"


int request(char *method, char *path){

    int fd, sizeFile;

    // Verifica qual é o tipo do recurso e adiciona-o na variável global mimeType
    verifyMimeType(path);

    if (strcmp(method, "GET") == 0){
        
        // Responde a operação no caminho solicitado e retorna o descritor do arquivo
        fd = httpResponse(path);

    }
    
    // Ler o arquivo e grava na variável global buffer
    sizeFile = read(fd, &buffer, sizeof(buffer));

    // Concatena o header com valores correspondentes ao arquivo
    snprintf(headers, sizeof(headers), "HTTP/1.1 %s\r\nContent-Type: %s\r\nContent-Length: %i\r\nConnection: close\r\n\r\n", status_msg, mimeType, sizeFile);

    // Fecha o arquivo
    close(fd);

    return 1;
}