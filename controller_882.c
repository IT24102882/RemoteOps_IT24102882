#include <stdio.h>
#include <stdlib.h>
#include <string.h>
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
    char command[1024];
    char response[1024];
    ssize_t bytes_received;
    
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

while (1)
{
printf("RemoteOps> ");

if (fgets(command, sizeof(command), stdin) == NULL)
{
    printf("\nController input closed.\n");
    break;
}

send(sock_fd, command, strlen(command), 0);

bytes_received = recv(sock_fd, response, sizeof(response) - 1, 0);

if (bytes_received > 0)
{
    response[bytes_received] = '\0';
    printf("Agent response: %s", response);
}
else
{
       printf("Agent disconnected.\n");
    break;
}
}
    close(sock_fd);
    return 0;
}
