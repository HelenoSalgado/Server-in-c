#ifndef CONFIG_H
#define CONFIG_H

// As variáveis de configuração agora são globais e podem ser modificadas em tempo de execução.
// Elas são declaradas aqui como 'extern' e definidas com valores padrão em 'config.c'.

extern const char *PORT;
extern char *DIR_ROOT;

#define _GNU_SOURCE
#define BUFFER_SIZE 2048
#define BUFFER_OUTPUT_SIZE 3000
#define MIMETYPE_SIZE 30
#define STATUS_SIZE 30
#define FILE_INDEX "/index.html"
#define PAGE_NOT_FOUND "/404.html" // O caminho será relativo ao DIR_ROOT

#endif // CONFIG_H