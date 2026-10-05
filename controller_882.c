#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define PORT 9410

int main(void)
{
    int sock_fd;
    struct sockaddr_in server_addr;

    printf("RemoteOps Controller starting...\n");
    printf("Connecting to TCP port: %d\n", PORT);

    sock_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (sock_fd < 0)
    {
        perror("socket failed");
        return 1;
    }

    printf("Controller TCP socket created successfully.\n");
      server_addr.sin_family = AF_INET;
      server_addr.sin_port = htons(PORT);

   if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0)
{
      perror("invalid address");
       close(sock_fd);
       return 1;
}
if (connect(sock_fd, (struct sockaddr *)&server_addr,
            sizeof(server_addr)) < 0)
{
    perror("connection failed");
    close(sock_fd);
    return 1;
}

printf("Connected to RemoteOps Agent successfully.\n");

    close(sock_fd);
    return 0;
}
