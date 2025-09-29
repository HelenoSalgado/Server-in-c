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

// Struct to pass connection data to the thread
typedef struct
{
  int client_fd;
  struct sockaddr_storage client_addr;
} connection_info;

void http_handler(http_context *ctx);
void print_usage(const char *prog_name);
void daemonize();

// The new connection handler function that will run in a separate thread
void *handle_connection(void *p_conn_info)
{
  connection_info *conn_info = (connection_info *)p_conn_info;
  int client_fd = conn_info->client_fd;

  // Detach the thread so that its resources are automatically released upon exit
  pthread_detach(pthread_self());

  http_context ctx;
  memset(&ctx, 0, sizeof(ctx));
  ctx.client_ip[0] = '\0'; // Ensure client_ip is always null-terminated
  ctx.client_fd = client_fd;

  // Extract client IP address
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

  free(p_conn_info); // Free the allocated memory for the argument

  http_handler(&ctx);
  log_request(&ctx);

  // The resulting concatenated data from the request is written to the client's file descriptor
  write(client_fd, ctx.data, strlen(ctx.data));

  // The client's file descriptor is closed
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

    // Fork off the parent process
    pid = fork();

    if (pid < 0) exit(EXIT_FAILURE); // Fork error
    if (pid > 0) exit(EXIT_SUCCESS); // Parent exits

    // Child process becomes session leader
    if (setsid() < 0) exit(EXIT_FAILURE);

    // Second fork to prevent process from acquiring a controlling terminal
    pid = fork();
    if (pid < 0) exit(EXIT_FAILURE);
    if (pid > 0) exit(EXIT_SUCCESS);

    // Change the file mode mask
    umask(0);

    // Change the current working directory
    chdir("/");

    // Close all open file descriptors
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

  // Resolve DIR_ROOT to an absolute path
  if (realpath(DIR_ROOT, absolute_dir_root) == NULL) {
      perror("Error resolving absolute path for DIR_ROOT");
      return 1;
  }
  DIR_ROOT = strdup(absolute_dir_root); // Duplicate the string as DIR_ROOT is const char*
  if (DIR_ROOT == NULL) {
      perror("Error duplicating DIR_ROOT string");
      return 1;
  }

  if (background)
  {
      daemonize();
  }

  init_logger(background);

  // Creates a socket and returns its descriptor
  int socket_descriptor = createSocket();

  printf("\x1b[36m\x1b[3m🚀 Server is ready to accept connections on port %s serving files from %s\n\x1b[0m", PORT, DIR_ROOT);

  for (;;)
  {
    connection_info *conn_info = malloc(sizeof(connection_info));
    if (!conn_info)
    {
      perror("Failed to allocate memory for connection info");
      continue;
    }

    socklen_t addr_size = sizeof(conn_info->client_addr);
    conn_info->client_fd = accept(socket_descriptor, (struct sockaddr *)&conn_info->client_addr, &addr_size);

    if (conn_info->client_fd < 0)
    {
      perror("Failed to accept connection");
      free(conn_info);
      continue;
    }

    pthread_t t;
    // Create a new thread to handle the connection
    if (pthread_create(&t, NULL, handle_connection, conn_info) != 0)
    {
      perror("Failed to create thread");
      close(conn_info->client_fd);
      free(conn_info);
    }
  }

  // This part is unreachable in the current infinite loop, but it's good practice
  close(socket_descriptor);
  free((char*)DIR_ROOT); // Free the duplicated string
  return 0;
}
