#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>

#include "config.h"
#include "socket/create.h"
#include "utils/regex.h"
#include "http/http_handler.h"
#include "http/context.h"

void http_handler(http_context *ctx);

int main()
{

  struct sockaddr_storage client_addr;
  socklen_t addr_size = sizeof(client_addr);

  // Creates a socket and returns its descriptor
  int socket_descriptor = createSocket();

  for (;;)
  {

    int client_fd = accept(socket_descriptor, (struct sockaddr *)&client_addr, &addr_size);

    if (client_fd > 0)
    {
      http_context ctx;
      memset(&ctx, 0, sizeof(ctx));
      ctx.client_fd = client_fd;

      http_handler(&ctx);

      // The resulting concatenated data from the request is written to the client's file descriptor
      write(client_fd, ctx.data, strlen(ctx.data));

      // The client's file descriptor is closed
      close(client_fd);
    }
  }

  return 0;
}
