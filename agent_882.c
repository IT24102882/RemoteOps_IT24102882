#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define PORT 9410
#define AUTH_TOKEN "OPS-2882"
#define SID "2882"

int main(void)
{
    int server_fd;
    int client_fd;
    int authenticated = 0;
    struct sockaddr_in server_addr;
     char buffer[1024];
     ssize_t bytes_received;
    FILE *uptime_file;
    double uptime_sec;
    FILE *mem_file;
    long mem_total;
    long mem_available;
    FILE *load_file;
    double cpu_load;



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

while (1)
{
bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);

if (bytes_received <= 0) {

    printf("Controller disconnected.\n");
    break;
}

buffer[bytes_received] = '\0';

printf("Received: %s\n", buffer);
if (authenticated == 0)
{

if (strcmp(buffer, "AUTH OPS-2882\n") == 0)
{
    authenticated = 1;
    printf("Authentication successful.\n");

    char response[] = "OK AUTHENTICATED SID:2882\n";
    send(client_fd, response, strlen(response), 0);
}
else
{
    printf("Authentication failed.\n");

    char response[] = "ERR 001 AUTH_FAILED SID:2882\n";
    send(client_fd, response, strlen(response), 0);
}
}

else
{
    if (strcmp(buffer, "SYSINFO\n") == 0)
    {
        printf("SYSINFO command received.\n");
        uptime_file = fopen("/proc/uptime", "r");

if (uptime_file != NULL)
{
    fscanf(uptime_file, "%lf", &uptime_sec);
    fclose(uptime_file);

    printf("System uptime: %.0f seconds\n", uptime_sec);
}
mem_file = fopen("/proc/meminfo", "r");

if (mem_file != NULL)
{
    fscanf(mem_file, "MemTotal: %ld kB\n", &mem_total);
    fscanf(mem_file, "MemFree: %*ld kB\n");
    fscanf(mem_file, "MemAvailable: %ld kB\n", &mem_available);

    fclose(mem_file);

    printf("Memory used: %ld MB\n",
           (mem_total - mem_available) / 1024);
}
load_file = fopen("/proc/loadavg", "r");

if (load_file != NULL)
{
    fscanf(load_file, "%lf", &cpu_load);
    fclose(load_file);

    printf("CPU load: %.2f\n", cpu_load);
}
char response[200];

snprintf(response, sizeof(response),
         "OK SYSINFO %.2f %ld %.0f SID:2882\n",
         cpu_load, (mem_total - mem_available) / 1024, uptime_sec);

send(client_fd, response, strlen(response), 0);    
}
else if (strcmp(buffer, "LISTPROC\n") == 0)
{
    printf("LISTPROC command received.\n");

    FILE *proc_file;
    char proc_buffer[2048] = "";
    char line[128];

    proc_file = popen("ps -eo pid=,comm=", "r");

    if (proc_file != NULL)
    {
        while (fgets(line, sizeof(line), proc_file) != NULL)
        {
            if (strlen(proc_buffer) + strlen(line) < sizeof(proc_buffer) - 20)
            {
                strcat(proc_buffer, line);
            }
        }

        pclose(proc_file);

        char response[2300];
        snprintf(response, sizeof(response),
                 "OK PROCS %s SID:2882\n", proc_buffer);

        send(client_fd, response, strlen(response), 0);
    }
}
    else
    {
        char response[] = "ERR 003 UNKNOWN_COMMAND SID:2882\n";
        send(client_fd, response, strlen(response), 0);
    }
}
}

    close(client_fd);

    close(server_fd); 
    return 0;
}

