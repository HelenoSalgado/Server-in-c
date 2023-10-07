#include "webserver.h"

int main(){

  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof(client_addr);
  struct tm *local;
  time_t t;

  t = time(NULL);
  local = localtime(&t);

  // Cria socket e devolve seu descritor
  int socket_descriptor = createSocket();

  printf("%s\n", "");
  printf("%s%d\n", "Server running in port: ", PORT);
  printf("%s\n", asctime(local));
  printf("%s\n", "");

  for(;;)
  {

    int client_fd = accept( socket_descriptor, (struct sockaddr *)&client_addr, &addr_size );

    if (client_fd > 0)
    {

      char bufferClient[BUFFER_SIZE], method[4];

      // Ler e reflete os dados enviados pelo client para o buffer do client
      recv(client_fd, &bufferClient, sizeof(bufferClient), 0);

      // Copia os três primeiros caracteres do buffer do client para method
      strncpy(method, bufferClient, 3);

      // Dado o buffer do cliente, o caminho do recurso é adicionado à variável global path
      regexPath(bufferClient);

      // Dado o método da operação e o caminho(path), a requisição é realizada
      request(method, path);

      // Concatena os dados a serem enviados para o cliente
      snprintf(data, sizeof(data), "%s%s", headers, buffer);

      // Os dados concatenados resultantes da requisação são escritos no arquivo descritor do cliente
      send(client_fd, data, strlen(data), 0);

      // Exibe o buffer do cliente no terminal
      printf("%s\n%s\n", asctime(local), bufferClient);

      // Redireciona saída para arquivo de log
      freopen(FILE_LOG, "a+", stdout);

      // Limpa a memória das variáveis globais
      memset(&headers, 0, sizeof(headers));
      memset(&buffer, 0, sizeof(buffer));
      memset(&data, 0, sizeof(data));
      memset(&path, 0, sizeof(path));

      // O arquivo descritor do cliente é fechado
      close(client_fd);
    
    }

  }

  return 1;
}
