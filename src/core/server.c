#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h> // Para umask
#include <arpa/inet.h> // Required Para inet_ntop
#include <pthread.h>
#include <limits.h> // Para PATH_MAX
#include <signal.h> // Para manipulação de sinais

#include "config.h"
#include "create.h"
#include "regex.h"
#include "cache.h"
#include "http_handler.h"
#include "context.h"
#include "logger.h"

// Cache global do servidor
cache_t server_cache;
pthread_mutex_t cache_mutex = PTHREAD_MUTEX_INITIALIZER;

// Variável global para controlar o loop do servidor
volatile sig_atomic_t server_running = 1;
int server_socket_fd = -1; // Socket principal para fechar no signal handler

// Thread de manutenção do cache
void *cache_maintenance_thread(void *arg) {
    (void)arg; // Suprime warning de parâmetro não usado
    
    while (server_running) {
        sleep(60); // Limpa cache a cada 1 minuto
        if (!server_running) break;
        
        pthread_mutex_lock(&cache_mutex);
        cache_cleanup(&server_cache);
        pthread_mutex_unlock(&cache_mutex);
    }
    return NULL;
}

// Estrutura para passar dados da conexão para a thread
typedef struct
{
  int client_fd;
  struct sockaddr_storage client_addr;
} connection_info;

void http_handler(http_context *ctx);
void print_usage(const char *prog_name);
void daemonize();
void signal_handler(int signal);

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
  printf("\nLogs:\n");
  printf("  When running in foreground: logs are displayed in the terminal\n");
  printf("  When running as daemon (-b): logs are saved to 'server.log' in the current directory and displayed in terminal\n");
}

void daemonize()
{
    pid_t pid;

    // Faz o fork do processo pai
    pid = fork();

    if (pid < 0) exit(EXIT_FAILURE); // Erro no fork
    if (pid > 0) {
        // Processo pai aguarda um pouco para o filho criar o arquivo PID
        sleep(1);

        // Lê o PID do arquivo criado pelo daemon
        FILE *pid_file = fopen("server.pid", "r");
        if (pid_file != NULL) {
            int daemon_pid;
            if (fscanf(pid_file, "%d", &daemon_pid) == 1) {
                printf("Daemon iniciado com PID: %d\n", daemon_pid);
            }
            fclose(pid_file);
        }
        exit(EXIT_SUCCESS);
    }

    // Processo filho se torna líder da sessão
    if (setsid() < 0) exit(EXIT_FAILURE);

    // Segundo fork para previnir que o processo adquira um terminal de controle
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    // Altera a máscara de modo de arquivo
    umask(0);

    // Mantém o diretório de trabalho atual para preservar caminhos relativos

    // Fecha todos os descritores de arquivo abertos
    for (int x = sysconf(_SC_OPEN_MAX); x >= 0; x--)
    {
        close(x);
    }
}

void signal_handler(int signal)
{
    if (signal == SIGINT || signal == SIGTERM) {
        server_running = 0;
        printf("\n\x1b[31mStopped\x1b[0m\n");

        // Limpa o cache
        cache_clear(&server_cache);
        printf("✓ Cache limpo\n");

        // Remove arquivo PID se existir
        unlink("server.pid");

        // Fecha o socket do servidor para interromper o accept()
        if (server_socket_fd != -1) {
            close(server_socket_fd);
        }

        exit(0); // Força o encerramento
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

      // Cria arquivo PID (o PID já foi exibido pelo processo pai)
      FILE *pid_file = fopen("server.pid", "w");
      if (pid_file != NULL) {
          fprintf(pid_file, "%d\n", getpid());
          fclose(pid_file);
      }
  }

  // Configura manipuladores de sinais para desligamento seguro
  signal(SIGINT, signal_handler);  // Ctrl+C
  signal(SIGTERM, signal_handler); // Comando kill

  init_logger(background, "server.log");

  // Inicializa o sistema de cache
  cache_init(&server_cache);
  printf("✓ Cache do servidor inicializado (TTL: %d segundos, máx: %d entradas)\n", CACHE_TTL_SECONDS, CACHE_MAX_ENTRIES);

  // Inicia thread de manutenção do cache
  pthread_t cache_thread;
  if (pthread_create(&cache_thread, NULL, cache_maintenance_thread, NULL) != 0) {
      perror("Falha ao criar thread de manutenção do cache");
      return 1;
  }
  pthread_detach(cache_thread);

  // Cria um socket e retorna seu descritor
  int socket_descriptor = createSocket();
  server_socket_fd = socket_descriptor; // Armazena globalmente para o signal handler

  printf("\x1b[36m\x1b[3m🚀 Servidor pronto para aceitar conexões na porta %s servindo arquivos de %s\n\x1b[0m", PORT, DIR_ROOT);

  while (server_running)
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
      if (server_running) { // Só mostra erro se o servidor ainda está rodando
        perror("Falha ao aceitar conexão");
      }
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

  // Limpeza e encerramento
  close(socket_descriptor);
  free((char*)DIR_ROOT); // Libera a string duplicada
  return 0;
}
