#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
//#include <netinet/in.h>
//#include <netdb.h>
#include <regex.h>

#include "config.h"
#include "http/create-socket.h"
#include "http/request.h"
//#include "http/verifyMimeType.h"

// Variáveis usadas em todo o escopo do programa
/// @brief 
char headers[500], 
     buffer[BUFFER_SIZE], 
     data[BUFFER_SIZE] = {0}, 
     mimeType[MIMETYPE_SIZE], 
     status_msg[STATUS_SIZE];

int main(int argc, char **argv){

  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof(client_addr);

  // Cria socket e devolve seu descritor
  int socket_descriptor = createSocket();


  for (;;)
  {

    int client_fd = accept( socket_descriptor, (struct sockaddr *)&client_addr, &addr_size );

    regex_t preg;
    char *pattern = "/([a-z0-9]*/?-?_?\\.?[a-z0-9]*)*";
    regmatch_t match[1];

    if (client_fd > 0)
    {

      char bufferClient[BUFFER_SIZE], method[4], path[50] = {0};

      // Ler e reflete os dados enviados pelo client para o buffer do client
      read(client_fd, bufferClient, sizeof(bufferClient));

      // Copia os três primeiros caracteres do buffer do client para method
      strncpy(method, bufferClient, 3);

      // Prepara um padrão regex para compilação
      regcomp(&preg, pattern, REG_EXTENDED | REG_ICASE);

      // A partir do padrão compilado, verifica se há correspondente no buffer do cliente 
      regexec(&preg, bufferClient, 1, match, 0);

      // Em caso afirmativo, copia da primeira à ultima posição correspondente na variável path
      for (int i = match[0].rm_so; i < match[0].rm_eo; i++){
        path[i - match[0].rm_so] = bufferClient[i];
      }

      // Dado o método da operação e o caminho(path), a requisição é realizada
      request(method, path);

      // Concatena os dados a serem enviados para o cliente
      snprintf(data, sizeof(data), "%s%s", headers, buffer);

      // Os dados concatenados resultantes da requisação são escritos no arquivo descritor do cliente
      write(client_fd, data, strlen(data));

      //Exibe o buffer do cliente no terminal
      //printf("%s\n", bufferClient);

      // A memória usada pelo padrão regex é liberada
      regfree(&preg);

      // Limpa a memória das variáveis globais
      memset(&headers, 0, sizeof(headers));
      memset(&buffer, 0, sizeof(buffer));
      memset(&data, 0, sizeof(data));

      // O arquivo descritor do cliente é fechado
      close(client_fd);
    
    }

  }

  return 1;
}
