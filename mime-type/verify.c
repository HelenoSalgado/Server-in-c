#include "./../config.h"
#include "verify.h"
#include <stdio.h>

int verifyMimeType(char *path){

    regex_t preg;
    char *pattern = "\\.[ css | js | svg | webp ]*", result[50];
    regmatch_t match[1];
    int found;

    // Prepara um padrão regex para compilação
    regcomp(&preg, pattern, REG_EXTENDED | REG_ICASE);

    // A partir do padrão compilado, verifica se há correspondente no buffer do cliente
    found = regexec(&preg, path, 1, match, 0);

    if (found == 0){

        // Em caso afirmativo, copia da primeira à ultima posição correspondente na variável path
        for (int i = match[0].rm_so; i < match[0].rm_eo; i++){
            result[i - match[0].rm_so] = path[i];
        }
       
    }else{

        strncpy(result, ".html", sizeof(result));
    }

    if(strcmp(result, ".html") == 0){
        strncpy(mimeType, "text/html", sizeof(mimeType));

    }else if (strcmp(result, ".css") == 0){
        strncpy(mimeType, "text/css", sizeof(mimeType));

    }
    else if (strcmp(result, ".js") == 0){
        strncpy(mimeType, "text/javascript", sizeof(mimeType));

    }
    else if (strcmp(result, ".svg") == 0){
        strncpy(mimeType, "image/svg+xml", sizeof(mimeType));

    }else if (strcmp(result, ".webp") == 0){
        strncpy(mimeType, "image/webp", sizeof(mimeType));

    }else{
        strncpy(mimeType, "application/octet-stream", sizeof(mimeType));
    }

    // Limpa variável result
    memset(&result, 0, sizeof(result));

    return 1;
};
