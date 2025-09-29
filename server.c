#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> // For umask
#include <arpa/inet.h> // Required for inet_ntop
#include <pthread.h>
#include <limits.h> // For PATH_MAX

#include "config.h"
#include "socket/create.h"
#include "utils/regex.h"
#include "http/http_handler.h"
#include "http/context.h"
#include "utils/logger.h"

// Estrutura para passar dados da conexão para a thread
typedef struct
{
  int client_fd;
  struct sockaddr_storage client_addr;
} connection_info;

void http_handler(http_context *ctx);
void print_usage(const char *prog_name);
void daemonize();

// O novo handler de conexão que rodará em uma thread separada
void *handle_connection(void *p_conn_info)
{
  connection_info *conn_info = (connection_info *)p_conn_info;
  int client_fd = conn_info->client_fd;

  // *** INÍCIO DA SEÇÃO DE SEGURANÇA: CONFIGURAÇÃO DE TIMEOUTS ***
  struct timeval timeout;
  timeout.tv_sec = 10; // 10 segundos de timeout
  timeout.tv_usec = 0;

  // Configura o timeout de recebimento (leitura)
  if (setsockopt(client_fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
      perror("setsockopt(SO_RCVTIMEO) failed");
      close(client_fd);
      free(conn_info);
      return NULL;
  }

  // Configura o timeout de envio (escrita)
  if (setsockopt(client_fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&timeout, sizeof(timeout)) < 0) {
      perror("setsockopt(SO_SNDTIMEO) failed");
      close(client_fd);
      free(conn_info);
      return NULL;
  }
  // *** FIM DA SEÇÃO DE SEGURANÇA ***

  // Desanexa a thread para que seus recursos sejam liberados automaticamente na saída
  pthread_detach(pthread_self());

  http_context ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.client_ip[0] = '\0'; // Garante que client_ip seja sempre terminado com nulo
  ctx.client_fd = client_fd;

  // Extrai o endereço IP do cliente
  if (conn_info->client_addr.ss_family == AF_INET)
  {
    struct sockaddr_in *s = (struct sockaddr_in *)&conn_info->client_addr;
    inet_ntop(AF_INET, &s->sin_addr, ctx.client_ip, sizeof(ctx.client_ip));
  }
  else // AF_INET6
  {
    struct sockaddr_in6 *s = (struct sockaddr_in6 *)&conn_info->client_addr;
    inet_ntop(AF_INET6, &s->sin6_addr, ctx.client_ip, sizeof(ctx.client_ip));
  }

  free(p_conn_info); // Libera a memória alocada para o argumento

  http_handler(&ctx);

  // Os dados concatenados resultantes da requisição são escritos no descritor de arquivo do cliente
  write(client_fd, ctx.data, strlen(ctx.data));

  // O descritor de arquivo do cliente é fechado
  close(client_fd);

  return NULL;
}

void print_usage(const char *prog_name)
{
  printf("Usage: %s [-p <port>] [-d <directory>] [-b] [-h]\n", prog_name);
  printf("  -p <port>      Specify the port to listen on (default: %s)\n", PORT);
  printf("  -d <directory> Specify the web root directory (default: %s)\n", DIR_ROOT);
  printf("  -b             Run as a daemon in the background\n");
  printf("  -h             Display this help message\n");
}

void daemonize()
{
    pid_t pid;

    // Faz o fork do processo pai
    pid = fork();

    if (pid < 0) exit(EXIT_FAILURE); // Erro no fork
    if (pid > 0) exit(EXIT_SUCCESS); // Processo pai termina

    // Processo filho se torna líder da sessão
    if (setsid() < 0) exit(EXIT_FAILURE);

    // Segundo fork para previnir que o processo adquira um terminal de controle
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    // Altera a máscara de modo de arquivo
    umask(0);

    // Altera o diretório de trabalho atual
    chdir("/");

    // Fecha todos os descritores de arquivo abertos
    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
    {
        close(x);
    }
}

int main(int argc, char *argv[])
{
  int opt;
  int background = 0;
  char absolute_dir_root[PATH_MAX];

  while ((opt = getopt(argc, argv, "p:d:bh")) != -1)
  {
    switch (opt)
    {
    case 'p':
      PORT = optarg;
      break;
    case 'd':
      DIR_ROOT = optarg;
      break;
    case 'b':
      background = 1;
      break;
    case 'h':
      print_usage(argv[0]);
      return 0;
    default: /* '?' */
      print_usage(argv[0]);
      return 1;
    }
  }

  // Resolve DIR_ROOT para um caminho absoluto
  if (realpath(DIR_ROOT, absolute_dir_root) == NULL) {
      perror("Erro ao resolver o caminho absoluto para DIR_ROOT");
      return 1;
  }
  DIR_ROOT = strdup(absolute_dir_root); // Duplica a string, pois DIR_ROOT é const char*
  if (DIR_ROOT == NULL) {
      perror("Erro ao duplicar a string DIR_ROOT");
      return 1;
  }

  if (background)
  {
      daemonize();
  }

  init_logger(background);

  // Cria um socket e retorna seu descritor
  int socket_descriptor = createSocket();

  printf("\x1b[36m\x1b[3m🚀 Servidor pronto para aceitar conexões na porta %s servindo arquivos de %s\n\x1b[0m", PORT, DIR_ROOT);

  for (;;)
  {
    connection_info *conn_info = malloc(sizeof(connection_info));
    if (!conn_info)
    {
      perror("Falha ao alocar memória para informações de conexão");
      continue;
    }

    socklen_t addr_size = sizeof(conn_info->client_addr);
    conn_info->client_fd = accept(socket_descriptor, (struct sockaddr *)&conn_info->client_addr, &addr_size);

    if (conn_info->client_fd < 0)
    {
      perror("Falha ao aceitar conexão");
      free(conn_info);
      continue;
    }

    pthread_t t;
    // Cria uma nova thread para lidar com a conexão
    if (pthread_create(&t, NULL, handle_connection, conn_info) != 0)
    {
      perror("Falha ao criar thread");
      close(conn_info->client_fd);
      free(conn_info);
    }
  }

  // Esta parte é inalcançável no loop infinito atual, mas é uma boa prática
  close(socket_descriptor);
  free((char*)DIR_ROOT); // Libera a string duplicada
  return 0;
}
