#include "regex.h"

int regexPath(char *bufferClient){

    extern char path[255];

    regex_t preg;
    char *pattern = "/([a-z0-9]*/?-?_?\\.?[a-z0-9]*)*";
    regmatch_t match[1];

    // Prepara um padrão regex para compilação
    regcomp(&preg, pattern, REG_EXTENDED | REG_ICASE);

    // A partir do padrão compilado, verifica se há correspondente no buffer do cliente 
    regexec(&preg, bufferClient, 1, match, 0);

    // Em caso afirmativo, copia da primeira à ultima posição correspondente na variável path
    for (int i = match[0].rm_so; i < match[0].rm_eo; i++){
       path[i - match[0].rm_so] = bufferClient[i];
    }

    // A memória usada pelo padrão regex é liberada
    regfree(&preg);

    //strncpy(path, processPath, sizeof(processPath));

    return 1;

}