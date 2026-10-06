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
if (strncmp(command, "PUT ", 4) == 0)
{
    char filename[100];

    if (sscanf(command, "PUT %99s", filename) == 1)
    {
        FILE *file = fopen(filename, "rb");

        if (file == NULL)
        {
            printf("File not found: %s\n", filename);
            continue;
        }

        fseek(file, 0, SEEK_END);
        long filesize = ftell(file);
        rewind(file);

        char put_command[256];
        snprintf(put_command, sizeof(put_command),
                 "PUT %s %ld\n", filename, filesize);
        send(sock_fd, put_command, strlen(put_command), 0);

        char file_buffer[1024];
        size_t bytes_read;

        while ((bytes_read = fread(file_buffer, 1,
                                   sizeof(file_buffer), file)) > 0)
        {
            send(sock_fd, file_buffer, bytes_read, 0);
        }

        fclose(file);

        bytes_received = recv(sock_fd, response,
                              sizeof(response) - 1, 0);

        if (bytes_received > 0)
    
   {
            response[bytes_received] = '\0';
            printf("Agent response: %s", response);
        }

        continue;
    }
}



if (strncmp(command, "GET ", 4) == 0)
{
    char filename[100];

    if (sscanf(command, "GET %99s", filename) == 1)
    {
        send(sock_fd, command, strlen(command), 0);

        bytes_received = recv(sock_fd, response,
                              sizeof(response) - 1, 0);

        if (bytes_received > 0)
        {
            response[bytes_received] = '\0';

            char received_filename[100];
            long filesize;

            if (sscanf(response,
                       "OK FILE_SEND %99s %ld SID:2882",
                       received_filename, &filesize) == 2)
            {
                printf("Agent response: OK FILE_SEND %s %ld SID:2882\n",
                       received_filename, filesize);

                FILE *file = fopen("downloaded_test.txt", "wb");

                if (file != NULL)
                {
                    long received = 0;
                    char file_buffer[1024];

                    while (received < filesize)
                    {
                        long remaining = filesize - received;
                        size_t to_receive =
                            remaining < (long)sizeof(file_buffer)
                            ? (size_t)remaining
                            : sizeof(file_buffer);

                        ssize_t n = recv(sock_fd, file_buffer,
                                         to_receive, 0);

                        if (n <= 0)
                            break;

                        fwrite(file_buffer, 1, n, file);
                        received += n;
                    }

                    fclose(file);

                    printf("File downloaded successfully: downloaded_test.txt\n");
                }
            }
            else
            {
                printf("Agent response: %s", response);
            }
        }

        continue;
    }
}
send(sock_fd, command, strlen(command), 0);

bytes_received = recv(sock_fd, response, sizeof(response) - 1, 0);

if (bytes_received > 0)
{
    response[bytes_received] = '\0';
    printf("Agent response: %s", response);

 if (strncmp(command, "QUIT", 4) == 0)
    {
        printf("Controller disconnected gracefully.\n");
        break;
    }



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
