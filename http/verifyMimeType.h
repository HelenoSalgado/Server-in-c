/*
* Verifica o tipo de recurso requerido pelo cliente e adiciona ao headers Content-Type
*/

#ifndef MIMITYPE
#define MIMITYPE

#include <string.h>
#include <regex.h>

// Variável global
extern char mimeType[MIMETYPE_SIZE];

// Pesquisa no caminho solicitado a extensão do arquivo e adiciona o tipo correspondente à variável global mimeType
int verifyMimeType(char *path);

#endif