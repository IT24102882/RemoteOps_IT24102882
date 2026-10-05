#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9410

int main(void)
{
    int server_fd;
    int client_fd;
    struct sockaddr_in server_addr;

    printf("RemoteOps Agent starting...\n");
    printf("TCP Port: %d\n", PORT);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("socket failed");
        return 1;
    }

    printf("TCP socket created successfully.\n");
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
{
        perror("bind failed");
        close(server_fd);
        return 1;
}

     printf("Socket bound to port %d successfully.\n", PORT);
     if (listen(server_fd, 5) < 0)
{
        perror("listen failed");
        close(server_fd);
        return 1;
}

    printf("Agent is listening on TCP port %d...\n", PORT);
    client_fd = accept(server_fd, NULL, NULL);

    if (client_fd < 0)
{
       perror("accept failed");
       close(server_fd);
       return 1;
}

   printf("Controller connected successfully.\n");

    close(client_fd);

    close(server_fd);
    return 0;
}



